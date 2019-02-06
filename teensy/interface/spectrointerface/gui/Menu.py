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

from PyQt5 import QtWidgets, QtGui

class Menu():
    """
    Class that represents the main menu in the main application window

    """
    _menuName = ""

    def __init__(self,menu,apw):
        self._menu = menu
        self._apw = apw
        self._buttonList = {} 

    @property
    def buttonList(self):
        return self._buttonList

    @classmethod
    def fromApplicationWindow(cls,apw):
        menu = apw.menuBar()
        return cls(menu,apw)

    @classmethod
    def fromParentMenu(cls,mm,apw):
        menu = mm.addMenu(cls._menuName)
        return cls(menu,apw)


class MainMenu(Menu):
    def __init__(self,menu,apw):
        """ 
        Constructor

        Populates an input application window.

        Parameters
        ----------
        apw: ApplicationWindow
            The main application window
        """
        super().__init__(menu,apw)
        
        self.__menuList = []
        self.__menuList.append(FileMenu.fromParentMenu(self._menu,apw))
        self.__menuList.append(CalibrateMenu.fromParentMenu(self._menu,apw))
        self.__menuList.append(HelpMenu.fromParentMenu(self._menu,apw))

    @property
    def menuList(self):
        return self.__menuList


class FileMenu(Menu):
    _menuName = "File"
    def __init__(self,mainMenu,apw):
        super().__init__(mainMenu,apw)

        ### Save settings
        settingsSaveButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-save"),'Save settings',apw)
        settingsSaveButton.setShortcut('Ctrl+S')
        settingsSaveButton.setStatusTip('Save settings')

        ### Exit button
        exitButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("application-exit"),'Exit',apw)
        exitButton.setShortcut('Ctrl+Q')
        exitButton.setStatusTip('Exit application')

        ### Add these to the menu
        self._menu.addAction(settingsSaveButton)
        self._menu.addAction(exitButton)

        ### Add the button to the list of buttons
        self._buttonList["settingsSave"] = settingsSaveButton
        self._buttonList["exit"] = exitButton

class HelpMenu(Menu):
    _menuName = "Help"
    def __init__(self,mainMenu,apw):
        super().__init__(mainMenu,apw)
        
        ### About
        aboutButton = QtWidgets.QAction(QtGui.QIcon.fromTheme(""),'About',apw)
        aboutButton.setStatusTip('About this application')

        self._menu.addAction(aboutButton)
        self._buttonList["about"] = aboutButton


class CalibrateMenu(Menu):
    _menuName = "Calibrate"
    def __init__(self,mainMenu,apw):
        super().__init__(mainMenu,apw)

        ### New calibration button
        newCalibrationButton = QtWidgets.QAction(QtGui.QIcon.fromTheme("document-new"),'Set calibration',apw)
        newCalibrationButton.setShortcut('Ctrl+N')
        newCalibrationButton.setStatusTip('Create/load new calibration table')

        self._menu.addAction(newCalibrationButton)

        self._buttonList["newCalibration"] = newCalibrationButton

