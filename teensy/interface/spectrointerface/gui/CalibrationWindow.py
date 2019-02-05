# Spectrometer Interface v0.1: a Python package that displays data gathered by
# the ILX526A Teensy interface. 
# 
# Copyright (C) 2019 Pierre-Yves Taunay
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

import numpy as np
import sys
from scipy.interpolate import interp1d

class CalibrationTableButtons(QtWidgets.QWidget):
    def __init__(self,mainWindow):
        QtWidgets.QWidget.__init__(self)

        ### Create buttons
        calcButton = QtWidgets.QPushButton('Calculate',self)
        addPixButton = QtWidgets.QPushButton('Add pixel',self)
        removePixButton = QtWidgets.QPushButton('Remove pixel',self)
        finishButton = QtWidgets.QPushButton('Finish',self)

        ### Add tool tips
        calcButton.setToolTip('Compute the calibration based on the current entries')
        addPixButton.setToolTip('Add another pixel to the list')
        removePixButton.setToolTip('Remove the last pixel from the list')
        finishButton.setToolTip('Finish calibration and use current table')

        ### Connect to buttons
        calcButton.clicked.connect(mainWindow.onCalculateClick)
        addPixButton.clicked.connect(mainWindow.onAddClick)
        removePixButton.clicked.connect(mainWindow.onRemoveClick)
        finishButton.clicked.connect(mainWindow.onFinishClick)

        ### Add layout
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(calcButton)
        layout.addWidget(addPixButton)
        layout.addWidget(removePixButton)
        layout.addWidget(finishButton)

        self.setLayout(layout)

        sspolicy = QtWidgets.QSizePolicy.Expanding
        self.setSizePolicy(sspolicy,sspolicy)

class CalibrationFileButtons(QtWidgets.QWidget):
    def __init__(self,mainWindow):
        QtWidgets.QWidget.__init__(self)

        ### Create buttons
        loadButton = QtWidgets.QPushButton('Load',self)
        saveButton = QtWidgets.QPushButton('Save',self)
        cancelButton = QtWidgets.QPushButton('Cancel',self)
        resetButton = QtWidgets.QPushButton('Reset',self)

        ### Set tool tips
        loadButton.setToolTip('Load an existing calibration table')
        saveButton.setToolTip('Save the current calibration table')
        resetButton.setToolTip('Reset the calibration table')
        cancelButton.setToolTip('Cancel calibration')

        ### Link signal
        loadButton.clicked.connect(mainWindow.onLoadClick)
        saveButton.clicked.connect(mainWindow.onSaveClick)
        cancelButton.clicked.connect(mainWindow.onCancelClick)
        resetButton.clicked.connect(mainWindow.onResetClick)

        ### Add to layout
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(loadButton)
        layout.addWidget(saveButton)
        layout.addWidget(resetButton)
        layout.addWidget(cancelButton)

        self.setLayout(layout)
        

class CalibrationButtons(QtWidgets.QWidget):
    def __init__(self,mainWindow):
        QtWidgets.QWidget.__init__(self)

        ### Create a layout
        layout = QtWidgets.QGridLayout()

        tableButtons = CalibrationTableButtons(mainWindow)
        fileButtons = CalibrationFileButtons(mainWindow)

        layout.addWidget(tableButtons,0,0)
        layout.addWidget(fileButtons,1,0)

        self.setLayout(layout)

class CalibrationWindow(QtWidgets.QWidget):

    __title = "Calibration"
    __left = 200
    __top = 200
    __width = 768 
    __height = 400 
    __calibrated = False

    def __init__(self):
        QtWidgets.QWidget.__init__(self)

        self.initUI()

        if self.__calibrated:
            self.displayCalibrationTable()
        else:
            self.initCalVec()


    def initUI(self):
        # Title
        self.setWindowTitle(self.__title)

        # Geometry
        self.setGeometry(self.__left,self.__top,self.__width,self.__height)

        # Set minimum size as the proposed geometry
        windowSize = self.size()
        self.setMinimumSize(windowSize)

        ### Populate the calibrationWindow
        ## Grid layout for the widgets
        layout = QtWidgets.QGridLayout()
        layout.setColumnStretch(0,4)
        layout.setColumnStretch(1,4)

        ## Create an information text box
        infostr =  "Enter the wavelength for each pixel that can be measured,"
        infostr += " or load an existing calibration table.\n"
        infostr += "When ready, hit 'Calculate' to compute the whole "
        infostr += "calibration table."
        infostr = str(infostr)

        infoText = QtWidgets.QTextEdit()
        infoText.setText(infostr)
        infoText.setReadOnly(True)

        layout.addWidget(infoText,0,0,1,2,QtCore.Qt.AlignCenter)
        # Adjust textbox size
        sspolicy = QtWidgets.QSizePolicy.Maximum
        infoText.setMinimumSize(self.__width-32,self.__height/4)
        infoText.setSizePolicy(sspolicy,sspolicy)

        ## Create a table
        self.tableWidget = QtWidgets.QTableWidget()
        self.tableWidget.setSizeAdjustPolicy(
                QtWidgets.QAbstractScrollArea.AdjustToContents)
        self.tableWidget.setRowCount(2)
        self.tableWidget.setColumnCount(2)

        # Change the header names
        header = self.tableWidget.horizontalHeader()
        for col in range(2):
            header.setSectionResizeMode(col,QtWidgets.QHeaderView.ResizeToContents)

        self.tableWidget.setHorizontalHeaderItem(0,QtWidgets.QTableWidgetItem("Pixel"))
        self.tableWidget.setHorizontalHeaderItem(1,QtWidgets.QTableWidgetItem("Wavelength (nm)"))

        # Remove the row numbers to avoid confusion
        self.tableWidget.verticalHeader().setVisible(False)
        

        # TODO: USER INPUT VALIDATION!!!!
        self.tableWidget.setItem(0,0, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.setItem(0,1, QtWidgets.QTableWidgetItem(""))

        layout.addWidget(self.tableWidget,1,0)

        self.tableWidget.setMinimumSize(self.__width/2,self.__height/2)
        self.tableWidget.setSizePolicy(sspolicy,sspolicy)

        ## Add buttons 
        self.calibrationButtons = CalibrationButtons(self)
        layout.addWidget(self.calibrationButtons,1,1)

        ### Save layout
        self.setLayout(layout)

    # TODO: REMOVE MAGIC NUMBER
    def initCalVec(self):
        self.calibration_table = np.zeros(3000)

    def displayCalibrationTable(self):
        self.tableWidget.setRowCount(0) # Delete all content
        self.tableWidget.setRowCount(3000)
        for pix in range(3000):
            str_pix = str(pix)
            str_wl = str(self.calibration_table[pix])
            self.tableWidget.setItem(pix,0,QtWidgets.QTableWidgetItem(str_pix))
            self.tableWidget.setItem(pix,1,QtWidgets.QTableWidgetItem(str_wl))

    def getCalibrationState(self):
        return self.__calibrated

    ### METHODS CALLED ON CLICK
    @QtCore.pyqtSlot()
    def onAddClick(self):
        nrow = self.tableWidget.rowCount()
        self.tableWidget.insertRow(nrow)

    @QtCore.pyqtSlot()
    def onRemoveClick(self):
        nrow = self.tableWidget.rowCount()
        self.tableWidget.removeRow(nrow-1)

    @QtCore.pyqtSlot()
    def onCalculateClick(self):
        # First reorder things
        self.tableWidget.sortItems(0) # Ascending order

        # Number of entries
        nrow = self.tableWidget.rowCount()
        holder = np.zeros((nrow,2),dtype=np.float64)

        # Role
        role = QtCore.Qt.DisplayRole

        # Iterate over rows
        for row in range(nrow):
            try:
                pix =  (int)(self.tableWidget.item(row,0).data(role))
                wl = (float)(self.tableWidget.item(row,1).data(role))

                if pix < 0 or pix >= 3000:
                    raise ValueError

                holder[row,:] = (pix,wl)
            except ValueError:
                errorMessage = "Pixel value is negative or greater than 2999"
                self.showErrorMessage(errorMessage)

            except:
                errorMessage = "Cannot read data at row "
                errorMessage += str(row+1)
                errorMessage += " : "
                errorMessage += str(sys.exc_info()[0])
                self.showErrorMessage(errorMessage)

        # Now, do a least squares fit 
        coeffs = np.polyfit(holder[:,0],holder[:,1],deg=1)
        pfit = np.poly1d(coeffs)

        # Get the fit evaluated at the pixels of interest
        pix_array = np.linspace(0,3000-1,3000)
        wl_array = pfit(pix_array)

        # Check the resulting array is correct
        if np.min(wl_array) < 200 or np.max(wl_array) > 1000:
            errorMessage = "Minimum value of resulting array is less than 200 nm or max. value is greater than 1,000. Check entries"
            self.showErrorMessage(errorMessage)
            return

        # Store calibration table
        self.calibration_table = wl_array

        self.displayCalibrationTable()

    @QtCore.pyqtSlot()
    def onFinishClick(self):
        # Make sure we calculate, even if it is unnecessary
        self.onCalculateClick()

        # Ensure that we store the calibration state
        self.__calibrated = True
        
        # Then quit
        self.close()


    @QtCore.pyqtSlot()
    def onCancelClick(self):
        self.close()

    ### TODO: HANDLE ERRORS BETTER HERE
    ### TODO: USE A LOGGER
    @QtCore.pyqtSlot()
    def onLoadClick(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        
        ffilter= "All Files (*);;Calibration file (*.cal)"
        try:
            # Open file and get calibration table
            fname, _ = QtWidgets.QFileDialog.getOpenFileName(self,"Open file","",ffilter,options=options)
            self.calibration_table = np.load(fname)
            self.displayCalibrationTable()
        except:
            errorMessage = "Could not open calibration file"
            self.showErrorMessage(errorMessage)

    @QtCore.pyqtSlot()
    def onSaveClick(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        
        ffilter= "All Files (*);;Calibration file (*.cal)"
        try:
            fname, _ = QtWidgets.QFileDialog.getSaveFileName(self,"Save file","",ffilter,options=options)
            np.save(fname,self.calibration_table)
        except:
            errorMessage = "Could not save calibration file"
            self.showErrorMessage(errorMessage)

    @QtCore.pyqtSlot()
    def onResetClick(self):
        self.tableWidget.setRowCount(0)
        self.tableWidget.setRowCount(1)

    ### TODO: Implement deletion of a given row

    def showErrorMessage(self,errorString):
        mb = QtWidgets.QMessageBox()
        mb.setIcon(QtWidgets.QMessageBox.Critical)
        mb.setWindowTitle("Error")
        mb.setText(errorString)
        mb.exec_()

