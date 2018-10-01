import numpy as np
import time
import serial
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/ttyACM0')
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
def read_buffer(data,start_frame):
    # Read a line and interpret as uint8
    line = ser.read(BUF_SIZE)
    current = np.frombuffer(line,dtype=np.uint8)

    # Find the location where the start of transmission occurs
    bool_indices = np.all(rolling_window(current, 32) == start_frame, axis=1)
    array_idx = np.mgrid[0:len(bool_indices)][bool_indices]

    # Get the data from the transmitted buffer, but skip the SOT 32 bytes 
    start_idx = array_idx[0] + 32

    # How many elements is that?
    nelem = len(current[start_idx::])

    # If we have less data than the total length of data required
    # then copy the data from the buffer, and flag that the array is not full yet 
    if nelem < NELEM_BYTES:
        np.copyto(data[0:nelem],current[start_idx::])
        partial_array = True

    # Otherwise copy 6200 elements and discard the garbage
    else:
        np.copyto(data,current[start_idx:start_idx+NELEM_BYTES])
        partial_array = False
        
    return nelem,partial_array

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
data = np.zeros(NELEM_BYTES,dtype=np.uint8)

nelem, partial_array = read_buffer(data,start_frame)

data16 = data.view(dtype=np.uint16) # This is of size 3100 pixels
data_V = np.zeros(NELEM,dtype=np.float64) # This is of size 3100 pixels

# Averaging quantities
NAVE = 10 # Number of frame averages
data_ave = np.zeros(NELEM,dtype=np.float64) 
idx = 0 

### PLOT
fig = plt.figure()
ax = fig.add_subplot(111)

li, = ax.plot(data16)
li_ave, = ax.plot(data_ave)

# Draw and show it
ax.relim() 
ax.autoscale_view(True,True,True)
fig.canvas.draw()
plt.show(block=False)

plt.ylim([0,5])
plt.xlim([0,NELEM])



while True:
    # If we had a partial array, then we grab the next buffer line and fill the rest with the beginning of the array
    if(partial_array):
        line = ser.read(BUF_SIZE)
        current = np.frombuffer(line,dtype=np.uint8)

        # How many elements did we miss?
        nadd = NELEM_BYTES-nelem
        np.copyto(data[nelem::],current[0:nadd])
        
        data16 = data.view(dtype=np.uint16)
        partial_array = False
    # Otherwise, the previous data was fine, so we can now try to fill the buffer again
    else:
        #print(data16,np.max(data16),data16.size,nelem,partial_array)
        # Convert to voltage and plot
        data_V = ADC_V / ADC_RES * data16 
        print(data16[0:100])

        li.set_ydata(data_V)
        fig.canvas.draw()

        # Accumulate the averaged data if we have not reached the number of samples
        if idx < NAVE:
            data_ave += data_V
            idx = idx + 1
        # Otherwise, calculate actual average and update plot
        else:
            idx = 0
            data_ave = data_ave / NAVE
            li_ave.set_ydata(data_ave)
            print(np.average(data_ave))
            data_ave = np.zeros(NELEM,dtype=np.float64) 
    
        # Reinitialize our data holder
        data = np.zeros(NELEM_BYTES,dtype=np.uint8)
        data16 = np.zeros(NELEM,dtype=np.uint16)
        nelem,partial_array = read_buffer(data,start_frame)
        data16 = data.view(dtype=np.uint16)

#    time.sleep(0.1)

