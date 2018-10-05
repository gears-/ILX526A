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

import numpy as np
import sys

from PyQt5 import QtCore

class DataReader(QtCore.QObject):
    # Constants
    SOT = "7eae261ddde24eeda293a7cd17e4379a" 
    NELEM = 3100
    NPIX = 3000
    NELEM_BYTES = 2*NELEM
    NPIX_BYTES = 2*NPIX
    BUF_SIZE = 6272

    ADC_RES = 4095
    ADC_V = 5

    # Threading
    dataReady = QtCore.pyqtSignal(bool) # is data available?
    dataDisplay = QtCore.pyqtSignal(np.ndarray) # actual data to display

    def __init__(self,*args,**kwargs):
        super().__init__()
        self.__abort = False

        self.start_frame = np.fromstring(self.SOT,dtype=np.uint8)
        # Holders for data
        self.data = np.zeros(self.BUF_SIZE,dtype=np.uint8) # Data for second half of packet
        self.data_p1 = np.zeros(self.BUF_SIZE,dtype=np.uint8) # Data for first half of packet
        self.continueFlag = True
        self.data_pix = np.zeros(self.NPIX,dtype=np.float64)
        self.partial_array = True

#        self.data16 = data[0:self.NELEM_BYTES].view(dtype=np.uint16) # This is of size 3000 pixels
#        self.data16_p1 = data_p1[0:self.NELEM_BYTES].view(dtype=np.uint16)

#        self.data_V = np.zeros(self.NELEM,dtype=np.float64) # This is of size 3000 pixels
#        self.data_plot = np.zeros(len(data16),dtype=np.float64)

    # See https://stackoverflow.com/questions/7100242/python-numpy-first-occurrence-of-subarray
    def rollingWindow(self,a, size):
        shape = a.shape[:-1] + (a.shape[-1] - size + 1, size)
        strides = a.strides + (a. strides[-1],)
        return np.lib.stride_tricks.as_strided(a, shape=shape, strides=strides)

    ### 
    # Function: read_buffer
    # Description: reads the buffer, then get the location where the start of transmission occurs
    # Modify the data vector in place
    # Returns:
    # - nelem: the number of elements from that subarray
    # - partial_array: a flag that tells us if we need to get data at the start of the next frame
    def readBuffer(self,data,ser):
        # Read a line and interpret as uint8
        line = ser.read(self.BUF_SIZE)
        current = np.frombuffer(line,dtype=np.uint8)

        # Find the location where the start of transmission occurs
        bool_indices = np.all(self.rollingWindow(current, 32) == self.start_frame, axis=1)
        array_idx = np.mgrid[0:len(bool_indices)][bool_indices]

        # TODO: Add check to see if bool_indices is empty or not. Maybe try/catch?
        if self.partial_array:
            end_idx = array_idx[0]
            nelem = len(current[0:end_idx])
            np.copyto(data[len(data)-nelem-32:len(data)-32],current[0:end_idx])
            self.partial_array = False

        else:
            start_idx = array_idx[-1] + 32
            nelem = len(current[start_idx::])
            self.partial_array = True
            try:
                np.copyto(data[0:nelem],current[start_idx::])
            except:
                np.copyto(data,current[start_idx::])
            
    @QtCore.pyqtSlot()
    def readUntil(self,ser,nread = 1):
        currentRead = 0
        print("Continuous read",currentRead,nread,self.continueFlag)

        while currentRead < nread and self.continueFlag:
            self.readBuffer(self.data,ser)
            self.readBuffer(self.data_p1,ser)


            data16 = self.data[0:self.NELEM_BYTES].view(dtype=np.uint16) # This is of size 3100 pixels
            data16_p1 = self.data_p1[0:self.NELEM_BYTES].view(dtype=np.uint16)

            # Full frame
            # 0-22: dummy signals
            # 23:52: optical black
            # 53:54: dummy 
            # 55:3054: data
            # 3054:end: dummy
            data_plot = 1.0*data16 + 1.0*data16_p1
            data_plot *= self.ADC_V / self.ADC_RES # Rescale in Volts
            data_plot = np.roll(data_plot,-3) # The ADC and CCD start at index -2; rolling this by -3 makes it start at "1" 

            # Extract data
            self.data_pix = data_plot[55:3055]
            self.data_black = data_plot[23:53]
            even_black = self.data_black[0::2]
            even_black = np.average(even_black)
            odd_black = self.data_black[0::2]
            odd_black = np.average(odd_black)

            # Raw data
            self.data_raw = self.data_pix
    
            self.data_pix[0::2] = self.data_pix[0::2] / odd_black
            self.data_pix[1::2] = self.data_pix[1::2] / even_black

            self.data_pix = np.abs(1-self.data_pix)

            data = np.zeros(self.BUF_SIZE,dtype=np.uint8) # Data for second half of packet
            data_p1 = np.zeros(self.BUF_SIZE,dtype=np.uint8) # Data for first half of packet
            currentRead = currentRead + 1

            self.dataDisplay.emit(self.data_pix)
            #self.dataReady.emit(True)


    def continuousRead(self,ser):
        print("Continuous read")
        self.readUntil(ser,nread=sys.maxsize)




