import numpy as np
import time
import serial
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/ttyACM0')
ser.flushInput()
np.set_printoptions(threshold=np.inf)

# See https://stackoverflow.com/questions/7100242/python-numpy-first-occurrence-of-subarray
def rolling_window(a, size):
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
def read_buffer(data,start_frame,partial_array):
    # Read a line and interpret as uint8
    line = ser.read(BUF_SIZE)
    current = np.frombuffer(line,dtype=np.uint8)

    # Find the location where the start of transmission occurs
    bool_indices = np.all(rolling_window(current, 32) == start_frame, axis=1)
    array_idx = np.mgrid[0:len(bool_indices)][bool_indices]

    # TODO: Add check to see if bool_indices is empty or not. Maybe try/catch?
    if partial_array:
        end_idx = array_idx[0]
        nelem = len(current[0:end_idx])
        np.copyto(data[len(data)-nelem-32:len(data)-32],current[0:end_idx])
        partial_array = False

    else:
        start_idx = array_idx[-1] + 32
        nelem = len(current[start_idx::])
        partial_array = True
        try:
            np.copyto(data[0:nelem],current[start_idx::])
        except:
            np.copyto(data,current[start_idx::])
        
    return nelem, partial_array

### CONSTANTS
# Start frame
SOT = "7eae261ddde24eeda293a7cd17e4379a" 
start_frame = np.fromstring(SOT,dtype=np.uint8)

# 
NELEM = 3100 # Number of elements from the CCD
NPIX = 3000 # Number of actual pixels
NELEM_BYTES = 2*NELEM # Number of elements from the CCD, in bytes 
BUF_SIZE = 6272 # Buffer size in bytes (98 packets of 64 bytes) 

# ADC and averaging
ADC_RES = 4095 # ADC resolution (12 bits)
ADC_V = 5 # ADC operating voltage (5 V)

# Holders for data
data = np.zeros(BUF_SIZE,dtype=np.uint8) # Data for second half of packet
data_p1 = np.zeros(BUF_SIZE,dtype=np.uint8) # Data for first half of packet

data16 = data[0:6200].view(dtype=np.uint16) # This is of size 3100 pixels
data16_p1 = data_p1[0:6200].view(dtype=np.uint16)

data_V = np.zeros(NELEM,dtype=np.float64) # This is of size 3100 pixels
data_plot = np.zeros(len(data16),dtype=np.float64)

#### PLOT
fig = plt.figure()
ax = fig.add_subplot(111)
#data_plot = 1.0*data16 + 1.0*data16_p1
li, = ax.plot(np.zeros(NPIX),'.')

# Draw and show it
ax.relim() 
ax.autoscale_view(True,True,True)
fig.canvas.draw()
plt.show(block=False)
plt.ylim([0,5])

pa = False # Partial array?
while True:
#for idx in np.arange(0,1,1):
    nelem, pa = read_buffer(data,start_frame,pa)
    nelem, pa = read_buffer(data_p1,start_frame,pa)

    data16 = data[0:6200].view(dtype=np.uint16) # This is of size 3100 pixels
    data16_p1 = data_p1[0:6200].view(dtype=np.uint16)

    # Full frame
    # 0-22: dummy signals
    # 23:52: optical black
    # 53:54: dummy 
    # 55:3054: data
    # 3054:end: dummy
    data_plot = 1.0*data16 + 1.0*data16_p1
    data_plot *= ADC_V / ADC_RES # Rescale in Volts
    data_plot = np.roll(data_plot,-3) # The ADC and CCD start at index -2; rolling this by -3 makes it start at "1" 

    # Extract data
    data_pix = data_plot[55:3055]
    data_black = data_plot[23:53]
    even_black = data_black[0::2]
    even_black = np.average(even_black)
    odd_black = data_black[0::2]
    odd_black = np.average(odd_black)

    data_pix[0::2] = data_pix[0::2] / odd_black
    data_pix[1::2] = data_pix[1::2] / even_black

    data = np.zeros(BUF_SIZE,dtype=np.uint8) # Data for second half of packet
    data_p1 = np.zeros(BUF_SIZE,dtype=np.uint8) # Data for first half of packet

    li.set_ydata(data_pix)
    fig.canvas.draw()


plt.show()

