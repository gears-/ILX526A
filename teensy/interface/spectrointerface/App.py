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

from PyQt5 import QtWidgets, QtCore, QtGui 

import sys
import numpy as np

from spectrointerface.gui.ApplicationWindow import ApplicationWindow
from spectrointerface.comm.USBCommunicator import USBCommunicator
from spectrointerface.process.ActionMap import ActionMap

from spectrointerface.comm.dataReader import DataReader

class calibrationWindow(QtWidgets.QWidget):
    def __init__(self,data=None):
        QtWidgets.QWidget.__init__(self)

        ### Populate the calibrationWindow
        ## Grid layout for the widgets
        layout = QtWidgets.QGridLayout()
        layout.setColumnStretch(0,4)
        layout.setColumnStretch(1,4)

        ## Create an information text box
        infoText = QtWidgets.QTextEdit()
        infoText.setText("Test!")
        infoText.setReadOnly(True)

        layout.addWidget(infoText,0,0,1,2,QtCore.Qt.AlignCenter)

        ## Create a table
        self.tableWidget = QtWidgets.QTableWidget()
        self.tableWidget.setRowCount(2)
        self.tableWidget.setColumnCount(2)

        # Change the header names
        self.tableWidget.setHorizontalHeaderItem(0,QtWidgets.QTableWidgetItem("Pixel"))
        self.tableWidget.setHorizontalHeaderItem(1,QtWidgets.QTableWidgetItem("Wavelength (nm)"))

        self.tableWidget.setItem(0,0, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.setItem(0,1, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.setItem(1,0, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.setItem(1,1, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.move(0,0)

        self.tableWidget.doubleClicked.connect(self.on_click)

        layout.addWidget(self.tableWidget,1,0)

        ## Add buttons 

        self.setLayout(layout)

    @QtCore.pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())

class App(QtWidgets.QApplication):
    """ 
    The master application that contains the application window and the corresponding user interface, and the actions that the UI triggers
    """

    # Some global class definitions
    sigDataStop = QtCore.pyqtSignal()

    def __init__(self,argv):
        super().__init__(argv)

        # Create a USB Communicator
        self.__USBCommunicator = USBCommunicator()

        # Create the UI
        self.__apw = ApplicationWindow()
        self.__apw.show()

        # Link UI to actions
        self.__actionMap = ActionMap(self)

    @property
    def apw(self):
        return self.__apw


    #=========
    # ACTIONS
    #=========
    def about(self):
        QtWidgets.QMessageBox.about(self.__apw, "About", """Interface with a spectrometer head that uses an ILX526A CCD array""")

    def calibrate(self):
        #QtWidgets.QMessageBox.about(self.__apw,"New calibration","""Calibrate the spectrometer""")
        self.w = calibrationWindow()
        self.w.show()
        #self.w.setWindowTitle("Calibration table")
        #self.w.setGeometry(QtCore.QRect(100, 100, 400, 200))
        #self.w.show()
        #self.w.setWindowModality(Qt.ApplicationModal)
        #self.w.exec_()
        # Open a new window with a table 
        # First column: wavelength
        # Second column: pixel
        
        # If the user needs to enter stuff, then propose to calibrate with interactive recording
        


    def updatePortList(self):
        # Get the ACM combo box
        inputACM = self.__apw.getToolbar("USB")._inputACM 

        # Clear the display
        inputACM.clear()

        # Refresh the port list on the communicator
        self.__USBCommunicator.refreshPortList()
        pl = self.__USBCommunicator.portList
        nelem = len(pl)
        
        # Are there any items? If yes, list them all
        if( len(pl) != 0):
            for idx in range(0,nelem):
                inputACM.addItem(pl[idx])
        else:
            inputACM.clear()
            errorMessage = "No suitable ports found!" + "\n"
            errorMessage += "Make sure the device is connected."
            self.showErrorMessage(errorMessage)

    def changeExposure(self):
        ### Change exposure if the usb communication channel is open
        text = self.__apw.toolbarList["CCD"].actionList["exposureChange"].text()
        
        # When we type stuff in we can remove the whole string or put spaces
        if text != "" and text != " ":
            textInt = int(text)

            if self.__USBCommunicator.isPortOpen:
                cmd = "e"
                if textInt > 1000:
                    cmd += "s"
                    cmd += str(textInt / 1000)
                else:
                    cmd += "m"
                    cmd += str(textInt)

                cmd += "\n"
                self.__USBCommunicator.sendCommand(cmd.encode())

            else:
                print("Port not open")
            

    def startAcquisition(self):
        ### Port tests
        # Get the ACM combo box
        inputACM = self.__apw.getToolbar("USB")._inputACM 

        # Check that the port initially chosen is available and not already open
        port = inputACM.currentText()

        # Attempt to open the port
        # Raise error and show error message if it fails to do so
        self.__threads = []
        self.__dataAcquisitionDone = 0
        try:
            # Open the port
            self.__USBCommunicator.openPort(port)

            # Setup a thread
            dataReader = DataReader()
            thread = QtCore.QThread()
            thread.setObjectName('datareader')
            self.__threads.append((thread,dataReader))
            dataReader.moveToThread(thread)

            # Control data acqusition
            self.sigDataStop.connect(dataReader.abort)

            # Get data from reader
            dataReader.dataDisplay.connect(self.onDataReady)

            # Start thread
            #readFunc = lambda : dataReader.readUntil(self.__USBCommunicator.ser,100)
            readFunc = lambda : dataReader.continuousRead(self.__USBCommunicator.ser)
            thread.started.connect(readFunc)
            thread.start()

        except Exception as error:
            self.showErrorMessage(str(error))

    @QtCore.pyqtSlot()
    def stopAcquisition(self):
        try:
            self.sigDataStop.emit()
            for thread, worker in self.__threads:
                thread.quit()
                thread.wait()
            self.__USBCommunicator.closePort()

        except Exception as error:
            self.showErrorMessage(str(error))

    @QtCore.pyqtSlot(np.ndarray)
    def onDataReady(self,data:np.ndarray):
        self.__apw._dc.updateFigure(data)

    def showErrorMessage(self,errorString):
        mb = QtWidgets.QMessageBox()
        mb.setIcon(QtWidgets.QMessageBox.Critical)
        mb.setWindowTitle("Error")
        mb.setText(errorString)
        mb.exec_()


