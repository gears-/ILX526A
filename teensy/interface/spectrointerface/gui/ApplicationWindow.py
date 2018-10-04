# Spectrometer Interface v0.1: a Python package that displays data gathered by
# the ILX526A Teensy interface. 
# 
# Copyright (C) 2018 Pierre-Yves Taunay
# 
# This program is free software: you can redistribute it andor modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https:/www.gnu.org/licenses/>.
# 
# Contact info: https:/github.com/pytaunay
# 
# Source: https:/github.com/pytaunay/ILX526A

from PyQt5 import QtCore, QtWidgets, QtGui 

from spectrointerface.comm.USBCommunicator import USBCommunicator
from spectrointerface.gui.SpectroGraph import SpectroGraph

class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        # Call parent function
        QtWidgets.QMainWindow.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)

        # Title
        self.title = "Spectrometer Interface 0.1"

        # Geometry
        self.left = 200
        self.top = 200
        self.width = 1024 
        self.height = 600

        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left,self.top,self.width,self.height)

        self.setupMenu()
        self.setupMainCanvas()

    def setupMainCanvas(self):
        ### Main widget
        self.main_widget = QtWidgets.QWidget(self)

        ### Matplotlib canvas
        l = QtWidgets.QVBoxLayout(self.main_widget)
        dc = SpectroGraph(self.main_widget, width=5, height=4, dpi=100)
        l.addWidget(dc)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)

    ### Menus
    def setupMenu(self):
        self.mainMenu = self.menuBar()
        self.mainToolBar = self.addToolBar("Main actions")
        self.inputToolBar = self.addToolBar("USB Control")
        self.ccdToolBar = self.addToolBar("CCD Control")
        self.acquireToolBar = self.addToolBar("Acquisition Control")

        self.setupFileMenu()
        self.setupCalibrateMenu()
        self.setupAboutMenu()
        self.setupCCDControl()
        self.setupAcquisition()

    def setupFileMenu(self):
        fileMenu = self.mainMenu.addMenu('File') 
        ### Save settings
        settingsSaveButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-save"),'Save settings',self)
        settingsSaveButton.setShortcut('Ctrl+S')
        settingsSaveButton.setStatusTip('Save settings')
        settingsSaveButton.triggered.connect(self.close) # TODO: IMplement settings save scheme
        fileMenu.addAction(settingsSaveButton)

        ### Exit button
        exitButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("application-exit"),'Exit',self)
        exitButton.setShortcut('Ctrl+Q')
        exitButton.setStatusTip('Exit application')
        exitButton.triggered.connect(self.close)
        fileMenu.addAction(exitButton)

        ### Toolbar
        self.mainToolBar.addAction(exitButton)
        self.mainToolBar.addAction(settingsSaveButton)

    def setupAboutMenu(self):
        self.mainMenu.addSeparator()
        aboutMenu = self.mainMenu.addMenu('Help')
        aboutMenu.addAction('&About',self.about)

    def about(self):
        QtWidgets.QMessageBox.about(self, "About", """Interface with a spectrometer head that uses an ILX526A CCD array""")

    def setupCalibrateMenu(self):
        calibrateMenu = self.mainMenu.addMenu('Calibrate')
        
        ### New calibration button
        newCalibrationButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-new"),'New calibration',self)
        newCalibrationButton.setShortcut('Ctrl+N')
        newCalibrationButton.setStatusTip('Create a new calibration table')
        newCalibrationButton.triggered.connect(self.calibrate)
        calibrateMenu.addAction(newCalibrationButton)

        ### Load calibration table button
        loadCalibrationButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-open"),'Load calibration table',self)
        loadCalibrationButton.setShortcut('Ctrl+L')
        loadCalibrationButton.setStatusTip('Load an existing calibration table')
        loadCalibrationButton.triggered.connect(self.calibrate)
        calibrateMenu.addAction(loadCalibrationButton)

        ### Toolbar
        self.mainToolBar.addAction(newCalibrationButton)
        self.mainToolBar.addAction(loadCalibrationButton)

    def calibrate(self):
        QtWidgets.QMessageBox.about(self,"New calibration","""Calibrate the spectrometer""")

    def updatePortList(self):
        self.USBCommunicator.refreshPortList()
        pl = self.USBCommunicator.portList
        nelem = len(pl)
        
        if( len(pl) != 0):
            for idx in range(0,nelem):
                self.inputACM.addItem(pl[idx])
        else:
            self.inputACM.clear()
            errorMessage = "No suitable ports found!" + "\n"
            errorMessage += "Make sure the device is connected."
            self.showErrorMessage(errorMessage)

    def setupCCDControl(self):
        ### Input port
        inputLabel = QtWidgets.QLabel(self)
        inputLabel.setText("Input port:")
        self.inputACM = QtWidgets.QComboBox()
        self.inputACM.setEditable(False)

        # Create a USB Communicator
        self.USBCommunicator = USBCommunicator()

        # Create a button that can be used to re-scan for ports
        scanPortButton = QtWidgets.QPushButton("Scan")
        scanPortButton.setToolTip("Rescan for ports")
        scanPortButton.clicked.connect(self.updatePortList)

        self.inputToolBar.addWidget(inputLabel)
        self.inputToolBar.addWidget(self.inputACM)
        self.inputToolBar.addWidget(scanPortButton)



        ### Exposure time
        exposureLabel = QtWidgets.QLabel(self)
        exposureLabel.setText("Exposure time (ms):")

        exposureValidator = QtGui.QIntValidator(5,1500)
        exposureValue = QtWidgets.QLineEdit(self)
        exposureValue.setValidator(exposureValidator)
        exposureValue.setText("5") # Default val.
        exposureValue.setFixedWidth(60)
        exposureValue.setAlignment(QtCore.Qt.AlignHCenter)

        self.ccdToolBar.addWidget(exposureLabel)
        self.ccdToolBar.addWidget(exposureValue)

        ### Number of averages
        averageLabel = QtWidgets.QLabel(self)
        averageLabel.setText("Number of averages")

        averageValidator = QtGui.QIntValidator(1,10)
        averageValue = QtWidgets.QLineEdit(self)
        averageValue.setValidator(averageValidator)
        averageValue.setText("1") # Default val.
        averageValue.setFixedWidth(60)
        averageValue.setAlignment(QtCore.Qt.AlignHCenter)

        self.ccdToolBar.addWidget(averageLabel)
        self.ccdToolBar.addWidget(averageValue)

    def setupAcquisition(self):
        playButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-playback-start"),'Start acquisition',self)
        stopButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-playback-stop"),'Stop acquisition',self)
        recButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-record"),'Record data',self)

        playButton.triggered.connect(self.startAcquisition)

        self.acquireToolBar.addAction(playButton)
        self.acquireToolBar.addAction(stopButton)
        self.acquireToolBar.addAction(recButton)

    def startAcquisition(self):
        ### Port tests
        # Check that the port initially chosen is available and not already open
        pl = self.USBCommunicator.portList
        nelem = len(pl) 

    def showErrorMessage(self,errorString):
        mb = QtWidgets.QMessageBox()
        mb.setIcon(QtWidgets.QMessageBox.Critical)
        mb.setWindowTitle("Error")
        mb.setText(errorString)
        mb.exec_()



