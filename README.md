ILX526A
------
The code in this repository is used to drive an ILX526A CCD array with an Arduino Uno or a Teensy. It assumes that the CCD array operates in the S/H mode.
The ILX526A documentation that details the timing requirements can be found [here](http://www.eureca.de/datasheets/01.xx.xxxx/01.01.xxxx/01.01.0021/ILX526A.pdf) 
An additional clock is generated to drive an ADC external to the CCD array.

The Arduino code makes use of direct port manipulation to generate the ROG and SHUT signals. The Teensy code relies on additional 16 bit timers for the generation of ROG and SHUT, with DMA accesses to manipulate incoming data. 

-----------------
The project was inspired by multiple sources: 
- Dave Allmon's [fast 16-bit spectrometer](http://davidallmon.com/pages/ad7667-spectrograph).
- erossel's [spectrometer](https://tcd1304.wordpress.com/)
