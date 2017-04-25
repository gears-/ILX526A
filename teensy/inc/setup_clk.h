#ifndef _SETUP_CLOCKS_H_
#define _SETUP_CLOCKS_H_

/****************************************************
 *            NUMBER OF PIXELS                      *
*****************************************************/
#define NPIX 3000 // Number of CCD pixels
#define NDUMMY0 57 // Number of dummy pixels before data stream
#define NDUMMY1 2 // Number of dummy pixels before SHUT up
#define BUF_SIZE (NPIX + 100) // Total buffer size to transfer

/****************************************************
 *            TIMING CONSIDERATIONS                 *
*****************************************************/
// Definitions for timing requirements of ROG and SHUT
// Assumes that the prescaler works out a clock of 12 MHz for the base clock of ROG and SHUT
// CCD clocks at 1 MHz => 1 us period

// Example for 250 ns delay:
// tau / 4 = 250 ns
// 250 ns delay corresponds to 3 cycles @ 12 MHz
// 250 / (1/12e6 * 1e9) = 3 

// Start ROG 250 ns after the main CCD clock: 3 cycles @ 12 MHz 
// Leave ROG up for 10 us: 120 cycles @ 12 MHz
#define ROG_UP 3 
#define ROG_DURATION (ROG_UP + 120)

// Number of cycles before the next CCD clock goes up 
// 750 ns: 9 cycles @ 12 MHz
#define ROG_NEXT_CCD_UP (ROG_DURATION + 9) 

// Number of cycles before SHUT drain goes up
// 3059 pixels @ 1 MHz = 36708 cycles @ 12 MHz 
// Add in 500 ns for 1/2 cycle of pixel D58, then 166 ns for timing requirements
// Corresponds to 6 cycles @ 12 MHz + 2 cycles @ 12 MHz
// Leave SHUT up for 5 us: 60 cycles @ 12 MHz
#define SHUT_UP (ROG_NEXT_CCD_UP + (NPIX + NDUMMY0 + NDUMMY1)*12 + 6 + 2)
#define SHUT_DURATION (SHUT_UP + 5*12) 



/****************************************************
 *            FUNCTION DECLARATION, MASKS           *
*****************************************************/
// Mask to disable the ADC on FTM1
#define ADC_MASK 0x02

// Mask to enable the PIT timer
#define PIT_START_MASK 0x01

// Function declarations
void ccd_clk(); 
void adc_clk(); 
void pwm_clk();
void setup_clk(); 

#endif
