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

class CalibrationTableButtons(QtWidgets.QWidget):
    def __init__(self,mainWindow):
        QtWidgets.QWidget.__init__(self)

        ### Create buttons
        calcButton = QtWidgets.QPushButton('Calculate',self)
        addPixButton = QtWidgets.QPushButton('Add pixel',self)
        removePixButton = QtWidgets.QPushButton('Remove pixel',self)

        ### Add tool tips
        calcButton.setToolTip('Compute the calibration based on the current entries')
        addPixButton.setToolTip('Add another pixel to the list')
        removePixButton.setToolTip('Remove the last pixel from the list')

        ### Connect to buttons
        calcButton.clicked.connect(mainWindow.onCalculateClick)
        addPixButton.clicked.connect(mainWindow.onAddClick)
        removePixButton.clicked.connect(mainWindow.onRemoveClick)

        ### Add layout
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(calcButton)
        layout.addWidget(addPixButton)
        layout.addWidget(removePixButton)

        self.setLayout(layout)

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
    def __init__(self,data=None):
        QtWidgets.QWidget.__init__(self)
        self.setWindowTitle("Calibration")

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

        ## Create a table
        self.tableWidget = QtWidgets.QTableWidget()
        self.tableWidget.setRowCount(2)
        self.tableWidget.setColumnCount(2)

        # Change the header names
        self.tableWidget.setHorizontalHeaderItem(0,QtWidgets.QTableWidgetItem("Pixel"))
        self.tableWidget.setHorizontalHeaderItem(1,QtWidgets.QTableWidgetItem("Wavelength (nm)"))

        self.tableWidget.setItem(0,0, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.setItem(0,1, QtWidgets.QTableWidgetItem(""))
        self.tableWidget.move(0,0)

        self.tableWidget.doubleClicked.connect(self.on_click)

        layout.addWidget(self.tableWidget,1,0)

        ## Add buttons 
        self.calibrationButtons = CalibrationButtons(self)
        layout.addWidget(self.calibrationButtons,1,1)

        ### Save layout
        self.setLayout(layout)

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
        print("TODO: Calculations")

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
            fname, _ = QtWidgets.QFileDialog.getOpenFileName(self,"Open file","",ffilter,options=options)
            print(fname)
        except:
            print("ERROR: Could not open calibration file")

    @QtCore.pyqtSlot()
    def onSaveClick(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        
        ffilter= "All Files (*);;Calibration file (*.cal)"
        try:
            # Open file
            fname, _ = QtWidgets.QFileDialog.getSaveFileName(self,"Save file","",ffilter,options=options)

            # Read content, then save data
            print(fname)
        except:
            print("ERROR: Could not save calibration file")

    @QtCore.pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())

