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

import serial
import glob

from PyQt5 import QtWidgets

class USBCommunicator():
    def __init__(self):
        # Find all possible serial ports 
        self.refreshPortList()

    def refreshPortList(self):
        ports = glob.glob('/dev/ttyACM[0-9]*')
        res = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                res.append(port)
            except:
                pass

        self.portList = res

    def openPort(self,port):
        # Attempt to open the port
        # Raise error if it fails to do so
        # The device name is stored in "ser"
        try:
            self.ser = serial.Serial(port)
            self.ser.flushInput()
            self.isPortOpen = True
        except Exception as error: 
            self.isPortOpen = False
            raise RuntimeError(str(error)) from error


    def closePort(self):
        try:
            self.ser.close()
            self.isPortOpen = False
        except Exception as error: 
            raise RuntimeError(str(error)) from error


