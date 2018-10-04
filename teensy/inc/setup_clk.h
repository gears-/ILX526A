/* ILX526A Teensy Interface v1.0: a C interface running on the Teensy v3.2 to 
 * obtain data from an ILX526A CCD that is digitized with an ADS803U ADC.
 *
 * Copyright (C) 2018 Pierre-Yves Taunay
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact info: https://github.com/pytaunay
 *
 * Source: https://github.com/pytaunay/ILX526A
 */
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
// Leave SHUT up to complete 3100 pixels, without counting the 10 pixels from ROG:
// - 3100 - (NPIX + NDUMMY0 + NDUMMY) = 41 => 41*12 = 492 cycles @ 12 MHz
#define SHUT_DUMMY (BUF_SIZE-NPIX-NDUMMY0-NDUMMY1)
#define SHUT_UP (ROG_NEXT_CCD_UP + (NPIX + NDUMMY0 + NDUMMY1)*12 + 6 + 2)
#define SHUT_DURATION (SHUT_UP + SHUT_DUMMY*12) 



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
