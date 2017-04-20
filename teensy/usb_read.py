import serial
import numpy as np
import sys

#ser = serial.Serial('/dev/ttyACM0',baudrate=9600,timeout=0)
#ser = serial.Serial('/dev/ttyACM0',baudrate=9600,timeout=None)
ser = serial.Serial('/dev/ttyACM0')

response = ""

# Wait for SOT
SOT = "7eae261d-dde2-4eed-a293-a7cd17e4379a\n"

SOT_TEST = "7eae261d-dde2-a293-a7cd17e4379a7eae261d-dde2-a293-a7cd17e4379aZ\n"
#while SOT_TEST != response:
#    response = ser.read(64)
#    print SOT_TEST,response
#    pass 

# Discard all of the stuff that made it to the serial port before we actually read it
ser.flushInput()
totdel = 0.0
NSAMPLES = 1000
for i in range(1,NSAMPLES):
    # Wait for the start of transmission
    start = ser.readline()
    while start != SOT:
        #print start 
        start = ser.readline()
        pass

    #discard = ser.read(2)
    #print "RESPONSE BELOW"
    #response = ser.readline()
    response = ser.read(2*3100)
    #response = ser.read(2*2)
    discard = ser.read(1)

    delay = ser.read(4)
    totdel += np.frombuffer(delay,np.uint32)
    discard = ser.read(1)
    response = np.frombuffer(response,np.uint16)
    #np.savetxt('test.dat',response,fmt='%d')

    #print response
    #print response.size
    #print np.frombuffer(delay,np.uint32)

totdel /= NSAMPLES
print "Average time (us): ",totdel
print "Bandwidth (MB/s): ", (2.*3100.+37.+1.+4.+1.)/totdel
print "Bandwidth (Mb/s): ", 8*(2.*3100.+37.+1.+4.+1.)/totdel

# Wait for the start of transmission
#start = ser.readline()
#while start != SOT:
#    #print start 
#    start = ser.readline()
#    pass
#
##discard = ser.read(2)
#print "RESPONSE BELOW"
##response = ser.readline()
#response = ser.read(2*3100)
#discard = ser.read(1)
#delay = ser.read(4)
#discard = ser.read(1)
#response = np.frombuffer(response,np.uint16)
#np.savetxt('test.dat',response,fmt='%d')
#
#print response
#print response.size
#print np.frombuffer(delay,np.uint32)

ser.close()



