#ifndef _SETUP_DMA_H_
#define _SETUP_DMA_H_

#include "DMAChannel.h"

#define NPIX 3100

volatile uint32_t pix_buffer[NPIX]; // Pixel buffer
volatile uint16_t pix_num; // Pixel number we are dealing with

// Create some DMA channels
DMAChannel dma_portc; // DMA for reading PORTC
//DMAChannel dma_pix_num; // DMA to update the pixel number we are dealing with 

DMAChannel dma_adc_start; // DMA to start the ADC
DMAChannel dma_adc_stop; // DMA to stop the ADC

// ADC Mask
static uint8_t adc_stop = 0xFF;
static uint8_t adc_start = 0x00;

// Function declaration
void setup_dma();
void setup_dma_adc();
void setup_dma_portc(); 

#endif
