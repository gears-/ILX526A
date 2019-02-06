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

from spectrointerface.gui.MplCanvas import MplCanvas

from PyQt5 import QtCore

import matplotlib.pyplot as plt
from matplotlib.widgets import Cursor
import numpy as np

class Cursor(object):
    def __init__(self, ax, canvas):
        self.ax = ax
        self.canvas = canvas
        self.lx = ax.axhline(color='k')  # the horiz line
        self.ly = ax.axvline(color='k')  # the vert line

        # text location in axes coords
        self.txt = ax.text(0.7, 0.9, '', transform=ax.transAxes)

    def mouse_move(self, event):
        if not event.inaxes:
            return

        x, y = event.xdata, event.ydata
        # update the line positions
        self.lx.set_ydata(y)
        self.ly.set_xdata(x)

        self.txt.set_text('x=%1.2f, y=%1.2f' % (x, y))
        self.canvas.draw()

class SnaptoCursor(object):
    """
    Like Cursor but the crosshair snaps to the nearest x,y point
    For simplicity, I'm assuming x is sorted
    """

    __calibration_table = None
    def __init__(self, ax, x, y, canvas):
        self.ax = ax
        self.canvas = canvas
        self.lx = ax.axhline(color='k')  # the horiz line
        self.ly = ax.axvline(color='k')  # the vert line
        self.x = x
        self.y = y

        # Text location in axes coords
        self.txt = ax.text(0.75, 0.9, '', transform=ax.transAxes)

    def mouse_move(self, event):

        if not event.inaxes:
            return

        x, y = event.xdata, event.ydata

        indx = min(np.searchsorted(self.x, [x])[0], len(self.x) - 1)
        x = self.x[indx]
        y = self.y[indx]
        try: 
            wl = self.__calibration_table[indx]
        except:
            wl = None
        
        
        # update the line positions
        self.lx.set_ydata(y)
        self.ly.set_xdata(x)
        
        try:
            self.txt.set_text('Pixel=%1.2f, Value=%1.2f\nWl=%1.2f nm' % (x, y, wl))
        except:
            self.txt.set_text('Pixel=%1.2f, Value=%1.2f' % (x, y))

        self.canvas.draw()

    def refreshData(self,x,y):
        self.x = x
        self.y = y

    def setCalibrationTable(self,calibration_table):
        self.__calibration_table = np.copy(calibration_table)


class SpectroGraph(MplCanvas):
    """A canvas that updates itself with a new plot."""

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        MplCanvas.__init__(self,parent,width,height,dpi)

    def computeInitialFigure(self):
        data = np.zeros(3000)
        self.li, = self.axes.plot(data,'.') 
        self.axes.relim()
        self.axes.autoscale_view(True,True,True)
        self.axes.set_ylim([0,1])

        self.cursor = SnaptoCursor(self.axes, self.li.get_xdata(),self.li.get_ydata(),self)
        self.mpl_connect('motion_notify_event',self.cursor.mouse_move)

        self.draw()
        plt.show()


    def updateFigure(self,data):
        self.li.set_ydata(data)
        self.cursor.refreshData(self.li.get_xdata(),self.li.get_ydata())
        self.draw()
        self.flush_events()

    # Calibration is stored in the cursor object
    def setCalibrationTable(self,calibration_table):
        self.cursor.setCalibrationTable(calibration_table)

