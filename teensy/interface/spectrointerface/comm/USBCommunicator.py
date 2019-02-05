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
import threading

from PyQt5 import QtWidgets,QtCore

from spectrointerface.comm.DataReader import DataReader

class USBCommunicator():
    """
    A class that is used to open / close the USB port

    Attributes:
        portList    the list of ttyACM ports available
        ser         the serial object obtained when opening a port
        __isPortOpen  a boolean to indicate if we opened a port
    """

    def __init__(self):
        # Find all possible serial ports 
        self.refreshPortList()
        self.__isPortOpen = False

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
            self.__isPortOpen = True
        except Exception as error: 
            self.__isPortOpen = False
            raise RuntimeError(str(error)) from error

    def closePort(self):
        try:
            self.ser.close()
            self.__isPortOpen = False
        except Exception as error: 
            raise RuntimeError(str(error)) from error

    def sendCommand(self,cmd):
        try:
            self.ser.write(cmd)
        except Exception as error: 
            raise RuntimeError(str(error)) from error

    @property
    def isPortOpen(self):
        return self.__isPortOpen

