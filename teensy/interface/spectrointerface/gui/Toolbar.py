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

from PyQt5 import QtWidgets, QtGui, QtCore

class Toolbar():
    """
    Class that represents the main toolbar in the main application window
    """
    _toolbarName = ""
    
    def __init__(self,apw):
        self._apw = apw
        self._toolbar = apw.addToolBar(self._toolbarName)
        self._buttonList = {}

    def getButtonList(self):
        return self._buttonList

#        self.__ccdToolBar = apw.addToolBar("CCD Control")
#        self.__acquireToolBar = apw.addToolBar("Acquisition Control")

#        self.setupAcquisition(apw)



class USBToolbar(Toolbar):
    _toolbarName = "USB Control"
    
    def __init__(self,apw):
        super().__init__(apw)
        # Creates the UI for the USB Input toolbar. It consists of a drop-down menu from where we can choose the USB ttyACM of interest, and a scan button to scan for available ports

        # Label 
        inputLabel = QtWidgets.QLabel(apw)
        inputLabel.setText("Input port:")

        # Combobox
        self.inputACM = QtWidgets.QComboBox()
        self.inputACM.setEditable(False)

        # Create a button that can be used to re-scan for ports
        scanPortButton = QtWidgets.QPushButton("Scan")
        scanPortButton.setToolTip("Rescan for ports")

        self._toolbar.addWidget(inputLabel)
        self._toolbar.addWidget(self.inputACM)
        self._toolbar.addWidget(scanPortButton)

        self._buttonList["scanPort"] = scanPortButton
#        scanPortButton.clicked.connect(apw.updatePortList)

class CCDControlToolbar(Toolbar):
    _toolbarName = "CCD Control"
    
    def __init__(self,apw):
        super().__init__(apw)

        ### Exposure time
        exposureLabel = QtWidgets.QLabel(apw)
        exposureLabel.setText("Exposure time (ms):")

        exposureValidator = QtGui.QIntValidator(5,1500)
        exposureValue = QtWidgets.QLineEdit(apw)
        exposureValue.setValidator(exposureValidator)
        exposureValue.setText("5") # Default val.
        exposureValue.setFixedWidth(60)
        exposureValue.setAlignment(QtCore.Qt.AlignHCenter)

        self._toolbar.addWidget(exposureLabel)
        self._toolbar.addWidget(exposureValue)

        ### Number of averages
        averageLabel = QtWidgets.QLabel(apw)
        averageLabel.setText("Number of averages")

        averageValidator = QtGui.QIntValidator(1,10)
        averageValue = QtWidgets.QLineEdit(apw)
        averageValue.setValidator(averageValidator)
        averageValue.setText("1") # Default val.
        averageValue.setFixedWidth(60)
        averageValue.setAlignment(QtCore.Qt.AlignHCenter)

        self._toolbar.addWidget(averageLabel)
        self._toolbar.addWidget(averageValue)


class AcquisitionToolbar(Toolbar):
    _toolbarName = "Acquisition"
    
    def __init__(self,apw):
        super().__init__(apw)

        playButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-playback-start"),'Start acquisition',apw)
        stopButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-playback-stop"),'Stop acquisition',apw)
        recButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("media-record"),'Record data',apw)


        self._toolbar.addAction(playButton)
        self._toolbar.addAction(stopButton)
        self._toolbar.addAction(recButton)

        self._buttonList["play"] = playButton
        self._buttonList["stop"] = stopButton
        self._buttonList["rec"] = recButton

#        stopButton.triggered.connect(self.stopAcquisition)
