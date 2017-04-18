#ifndef _SETUP_DMA_H_
#define _SETUP_DMA_H_

#define NPIX 3000

#include <Arduino.h>
#include "DMAChannel.h"

// External variables shared across multiple files
// Pixel buffer
volatile uint16_t pix_buffer[6*(NPIX+100)];

// DMA Channels
DMAChannel dma_portc; // DMA for reading PORTC
DMAChannel dma_adc_start; // DMA to start the ADC
DMAChannel dma_adc_stop; // DMA to stop the ADC


// Function declaration
void setup_dma();
void setup_dma_adc();
void setup_dma_portc(); 

#endif
