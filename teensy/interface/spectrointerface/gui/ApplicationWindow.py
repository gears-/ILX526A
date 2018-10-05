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
from spectrointerface.comm.dataReader import DataReader

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
    def __init__(self):
        """
        Constructor

        Initializes the user interface and the corresponding actions.
        """
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


        # Initialize the user interface
        self.initUI()

    def initUI(self):
        # Title
        self.setWindowTitle(self.title)

        # Geometry
        self.setGeometry(self.left,self.top,self.width,self.height)
        # Add a status bar
        self.statusBar()

        # Add a menu
        self.__menu = MainMenu.fromApplicationWindow(self) # Create the main menu bar

        # Create toolbars
        self.__toolbarList = []
        self.__toolbarList.append(USBToolbar(self))
        self.__toolbarList.append(CCDControlToolbar(self))
        self.__toolbarList.append(AcquisitionToolbar(self))

        # Setup the main graph
        #self.setupMainCanvas()

    def getMainMenu(self):
        return self.__menu

    def getToolbarList(self):
        return self.__toolbarList

    def setupMainCanvas(self):
        ### Main widget
        self.main_widget = QtWidgets.QWidget(self)

        ### Matplotlib canvas
        l = QtWidgets.QVBoxLayout(self.main_widget)
        self.dc = SpectroGraph(self.main_widget, 
                width=5, height=4, dpi=100,
                comm = self.USBCommunicator)
        l.addWidget(self.dc)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)

