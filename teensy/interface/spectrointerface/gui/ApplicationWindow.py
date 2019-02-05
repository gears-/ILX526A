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
from spectrointerface.comm.DataReader import DataReader

from spectrointerface.gui.SpectroGraph import SpectroGraph
from spectrointerface.gui.Menu import MainMenu
from spectrointerface.gui.Toolbar import USBToolbar, CCDControlToolbar, AcquisitionToolbar


import numpy as np

class ApplicationWindow(QtWidgets.QMainWindow):
    """
    Main application window. 

    Contains the menus, toolbars, etc.

    Attributes
    ----------

    Methods
    -------

    """
    # Title
    __title = "Spectrometer Interface 0.1"

    # Geometry
    __left = 200
    __top = 200
    __width = 1024 
    __height = 600

    def __init__(self):
        """
        Constructor

        Initializes the user interface and the corresponding actions.
        """
        # Call parent function
        QtWidgets.QMainWindow.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)

        # Initialize the user interface
        self.initUI()

    def initUI(self):
        # Title
        self.setWindowTitle(self.__title)

        # Geometry
        self.setGeometry(self.__left,self.__top,self.__width,self.__height)
        # Add a status bar
        self.statusBar()

        # Add a menu
        self.__menu = MainMenu.fromApplicationWindow(self) # Create the main menu bar

        # Create toolbars
        self.__toolbarList = {} 
        self.__toolbarList["USB"] = USBToolbar(self)
        self.__toolbarList["CCD"] = CCDControlToolbar(self)
        self.__toolbarList["acquisition"] = AcquisitionToolbar(self)

        # Setup the main graph
        self.setupMainCanvas()

    def setupMainCanvas(self):
        ### Main widget
        self.__mainWidget = QtWidgets.QWidget(self)

        ### Matplotlib canvas
        layout = QtWidgets.QVBoxLayout(self.__mainWidget)
        self._dc = SpectroGraph(self.__mainWidget, width=5, height=4, dpi=100)
        layout.addWidget(self._dc)

        self.__mainWidget.setFocus()
        self.setCentralWidget(self.__mainWidget)

    @property
    def menu(self):
        return self.__menu

    @property
    def toolbarList(self):
        return self.__toolbarList

    def getToolbar(self,name):
        return self.__toolbarList[name]



