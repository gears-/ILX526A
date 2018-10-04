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

import matplotlib.pyplot as plt

class SpectroGraph(MplCanvas):
    """A canvas that updates itself with a new plot."""

    def __init__(self, *args, **kwargs):
        MplCanvas.__init__(self, *args, **kwargs)
        #timer = QtCore.QTimer(self)
        #timer.timeout.connect(self.update_figure)
        #timer.start(1000)

    # Compute the initial figure from data stored in the communicator
    def computeInitialFigure(self,comm):
        data = comm.dataReader.data_pix
        self.li, = self.axes.plot(data,'.') 
        self.axes.relim()
        self.axes.autoscale_view(True,True,True)
        self.axes.set_ylim([0,1])
        
    def updateFigure(self):
        data = comm.dataReader.data_pix
        self.li.set_ydata(data)
        self.draw()


