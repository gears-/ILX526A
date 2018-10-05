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

class ActionMap():
    """
    Class that links the UI buttons to the actual app actions
    """

    def __init__(self,qApp):
        # Application window
        apw = qApp.apw

        # List and link all menu buttons
        mainMenu = apw.menu 
        for menu in mainMenu.menuList:
            bList = menu.buttonList
            for button in bList:
                if button == "settingsSave":
                    bList[button].triggered.connect(apw.close)
                elif button == "exit":
                    bList[button].triggered.connect(apw.close)
                elif button == "newCalibration":
                    bList[button].triggered.connect(qApp.calibrate)
                elif button == "loadCalibration":
                    bList[button].triggered.connect(qApp.calibrate)
                elif button == "about":
                    bList[button].triggered.connect(qApp.about)


        # List and link all toolbar buttons
        toolbarList = apw.toolbarList
        for tb in toolbarList:
            aList = toolbarList[tb].actionList

            for action in aList:
                if action == "scanPort":
                    aList[action].clicked.connect(qApp.updatePortList)
                elif action == "play":
                    aList[action].triggered.connect(qApp.startAcquisition)
                elif action == "stop":
                    aList[action].triggered.connect(qApp.stopAcquisition)
                elif action == "exposureChange":
                    aList[action].textChanged.connect(qApp.changeExposure)

