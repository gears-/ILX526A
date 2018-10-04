from PyQt5 import QtCore, QtWidgets, QtGui

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
        self.width = 800
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
        self.setupFileMenu()
        self.setupCalibrateMenu()
        self.setupAboutMenu()

    def setupFileMenu(self):
        fileMenu = self.mainMenu.addMenu('File') 
        ### Save settings
        settingsSaveButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-save"),'Save settings',self)
        settingsSaveButton.setShortcut('Ctrl+S')
        settingsSaveButton.setStatusTip('Save settings')
        settingsSaveButton.triggered.connect(self.close) # TODO: IMplement settings save scheme
        fileMenu.addAction(settingsSaveButton)

        # Link the defined short
        settingsSaveButtonShortcut = QtWidgets.QShortcut(QtGui.QKeySequence("Ctrl+S"),self)
        settingsSaveButtonShortcut.activated.connect(self.close) 

        ### Exit button
        exitButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("application-exit"),'Exit',self)
        exitButton.setShortcut('Ctrl+Q')
        exitButton.setStatusTip('Exit application')
        exitButton.triggered.connect(self.close)
        fileMenu.addAction(exitButton)

        # Link the defined short
        exitButtonShortcut = QtWidgets.QShortcut(QtGui.QKeySequence("Ctrl+Q"),self)
        exitButtonShortcut.activated.connect(self.close)


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

        newCalibrationButtonShortcut = QtWidgets.QShortcut(QtGui.QKeySequence("Ctrl+N"),self)
        newCalibrationButtonShortcut.activated.connect(self.calibrate) 

        ### Load calibration table button
        loadCalibrationButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-new"),'New calibration',self)
        loadCalibrationButton.setShortcut('Ctrl+N')
        loadCalibrationButton.setStatusTip('Create a new calibration table')
        loadCalibrationButton.triggered.connect(self.calibrate)
        calibrateMenu.addAction(loadCalibrationButton)

        loadCalibrationButtonShortcut = QtWidgets.QShortcut(QtGui.QKeySequence("Ctrl+L"),self)
        loadCalibrationButtonShortcut.activated.connect(self.calibrate) 

    def calibrate(self):
        QtWidgets.QMessageBox.about(self,"New calibration","""Calibrate the spectrometer""")

