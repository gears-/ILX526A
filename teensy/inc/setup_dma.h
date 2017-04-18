#ifndef _SETUP_DMA_H_
#define _SETUP_DMA_H_

#include <Arduino.h>
#include "DMAChannel.h"

#define NPIX 3000 // Number of CCD pixels
#define NBIT 12 // Number of ADC bits

// Function declaration
void setup_dma();
void setup_dma_adc();
void setup_dma_portc(); 

#endif
