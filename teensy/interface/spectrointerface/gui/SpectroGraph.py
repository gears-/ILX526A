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
import numpy as np

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
        plt.show()
        
    def updateFigure(self,data):
        self.li.set_ydata(data)
        self.draw()
        self.flush_events()


