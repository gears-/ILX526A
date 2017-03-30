ILX526A
------
The code in this repository is used to drive an ILX526A CCD array with an Arduino Uno. It assumes that the CCD array operations in S/H mode, at max speed (1 MHz).
The ILX526A documentation that details the timing requirements can be found [here](http://www.eureca.de/datasheets/01.xx.xxxx/01.01.xxxx/01.01.0021/ILX526A.pdf) 

An additional clock at 4 MHz is generated to drive an ADC external to the CCD array. 

The 4 MHz and 1 MHz clocks are generated using timer0 and timer2, respectively. 
Timer0 outputs 4 MHz on the Uno digital pin 6 using OC0A. 
Timer1 outputs 1 MHz on the Uno digital pin 3 using OC2B.

Direct port manipulation of PORTD is used to 
1. make sure the CCD array is driven as fast as possible, 
2. ensure simplicity (digital ports 0 through 7 only are used). 

 The pin assignment on the Uno is as follows
| Digital Pin | Function | 
|:-------------: |:-------------:| 
| PD0 | NC |
| PD1 | NC |
| PD2 | ROG |
| PD3 | CCD Clock @ 1 MHz |
| PD4 | SHUT |
| PD5 | NC | 
| PD6 | ADC Clock @ 4 MHz |
| PD7 | Debugging | 


This is a work in progress. The driving board may change from an Arduino to other micro-controllers.

