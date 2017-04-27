ILX526A
------
The code in this repository is used to drive an ILX526A CCD array with a Teensy 3.2. It assumes that the CCD array operates in the S/H mode.
The ILX526A documentation that details the timing requirements can be found [here](http://www.eureca.de/datasheets/01.xx.xxxx/01.01.xxxx/01.01.0021/ILX526A.pdf) 
An additional clock is generated to drive an ADC (ADS803U) external to the CCD array. 

The datasheet for the Cortex M4 processor that equips the Teensy 3.2 (MK20DX256VLH7) can be found [here](https://www.pjrc.com/teensy/K20P64M72SF1RM.pdf). 

### Cycle
The CCD is grabbing data continuously. The CCD data is digitized after the read-out gate (ROG) goes high. This marks the start of the cycle. 
Pixels are shifted out one by one on each CCD clock cycle, until the shutter drain (SHUT) goes high. Exposure starts when SHUT goes LOW. 

### Flexible timers (FTM)
The code uses all available flexible timers to generate clocks and recurring signals, and makes sure to synchronize them. 
At full speed, both the ADC and CCD are driven at 1 MHz. ROG and SHUT are as fast as can be based on the timing requirements. 


#### Channels
The table below shows the total number of channels for each flexible timer

| Flexible timer | Channels |
|:--------------:|:--------:|
| FTM0 | 8 |
| FTM1, FTM2 | 2 |

FTM1 is used to generate clocks for the CCD and ADC. Since the external ADC used (ADS803U) requires 6 clock cycles before data is available, garbage data is gathered at first in the data buffer. 

FTM0 uses 4 channels for the ROG and SHUT PWM signals. The channels are used in pairs, using the COMBINE mode.   

#### Synchronization
A nifty feature from the K20 ARM processor is _global timer base_ (GTB). GTB enables the FTM signals to be synchronized. Note that only FTM0 can be used as the master (see Section 3.8.2.9 p116)!!!  


### Periodic Interrupt Timer (PIT)
PIT0 is used to control the exposure time. It is a 32 bit timer. Assuming the bus clock is at its maximum (48 MHz), this corresponds to about 90s maximum of exposure time. 


### Direct Memory Accesses (DMA)
The DMA engine is used to trigger events very fast. On the K20 ARM processor, multiple DMA requests may be linked, which allows for complex behavior. The following DMA requests are executed:
1. Each time the ADC clock goes HIGH, the whole PORTC bus is transferred into the data buffer
2. When ROG goes LOW, the ADC is enabled
3. On a linked channel, the most current counter value is loaded into PIT_LDVAL0 
4. When SHUT goes LOW, the FTM clocks are disabled by manipulating the SIM_SCGC6 register 
5. On a linked channel, PIT0 is started
6. On a linked channel copy the data from the data buffer to the buffer staged for data transfer 

### ISR
Both the ROG and SHUT DMA raise ISRs. Each channel activates the other one. For example, when the ROG ISR is called, ROG disables itself, and enables SHUT. The SHUT DMA also disables the ADC during the exposure time.  
PIT0 triggers an ISR, which re-starts the FTM clock for the next cycle.

### Data transfer
Data is marked for transfer with a DMA. The transfer is done through the USB "serial" interface of the Teensy. A header (originally randomly generated) is sent to the host, then the data stream begins. The total time it takes to transfer the data is also communicated to the host.  

### Output Pins
The flexible timers provide a pin output on the Teensy. See Teensy's documentation to map out the K20 ports to the Teensy pins. The ports are configured with the PORTx_PCRn register on the K20 (see p.227). The field of interest is "MUX" (bits 10-8), which lists "alternative configurations". The Table p207 shows the "alternative configurations". For example, PORTB0 corresponds to GPIO pin 16 on the Teensy if configured with MUX set to 1. The same pin may be used to drive FTM1_CH0 if MUX is set to 3. 
As of 04/26/2017, the following pins are used to generate all of the clock signals

| Port ID | Teensy pin | Function | Usage | 
|:-------:|:----------:|:--------:|:-----:|
| PA12 | 3 | FTM1_CH0 | CCD Clock |
| PB1 | 17 | FTM1_CH1 | ADC Clock |
| PD4 | 6 | FTM0_CH4 | ROG PWM |
| PD7 | 5 | FTM1_CH7 | SHUT PWM | 
| PC0 | 15 | ADC B0 | Bit 0 from ADC |
| PC1 | 22 | ADC B1 | Bit 1 from ADC |
| PC2 | 23 | ADC B2 | Bit 2 from ADC |
| PC3 | 9 | ADC B3 | Bit 3 from ADC |
| PC4 | 10 | ADC B4 | Bit 4 from ADC |
| PC5 | 13 | ADC B5 | Bit 5 from ADC |
| PC6 | 11 | ADC B6 | Bit 6 from ADC |
| PC7 | 12 | ADC B7 | Bit 7 from ADC |
| PC8 | 28 | ADC B8 | Bit 8 from ADC |
| PC9 | 27 | ADC B9 | Bit 9 from ADC |
| PC10 | 29 | ADC B10 | Bit 10 from ADC |
| PC11 | 30 | ADC B11 | Bit 11 from ADC |


### Clocks
FTM operates on the _bus clock_, not the system clock. Section 5 of the documentation outlines the clock definitions and requirements
1. Core and system clocks must be 72 MHz or less
2. Bus clock must be _less than 50 MHz, and an integer divide of the core clock_
3. Flash clock must be 25 MHz or less, less than or equal to the bus clock, and integer divide of the core clock.  
The table below outlines the different bus clock values one can obtain based on the core clock.

| Core clock (MHz) | Bus clock (MHz) |
|:----------:|:---------:|
| 96 | 48 |
| 72 | 36 |
| 48 | 48 |

The maximum signal one can generate from FTM is 1/2 of the bus clock. This corresponds to 24 MHz.


### FAQ
1. Why not use DMA with PIT timers in lieu of FTM? 
> It seems that the maximum switching frequency of a port based on DMA requests triggered by PIT timers is 3 MHz. There is _no_ guarantee (?) of synchronization between FTM and PIT, or between multiple PIT channels. The output of a 1 MHz clock from FTM shifts in time with respect to the 3 MHz PIT. Jitter also shows up on a scope. 



-----------------
