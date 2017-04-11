/*
 * File: ccd_clk.c
 * Purpose: set up the CCD clock for the SP1 Spectrometer
 * The CCD clock is maxed at 1 MHz. However, to synchronize correctly with the ADS803U ADC (max. freq 5 MHz), 
 * we chose an ADC frequency of 4.8 MHz, and a CCD frequency 1/6th of the ADC (800 kHz).
 */

 void ccd_clk() {}
 
