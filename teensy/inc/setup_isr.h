#ifndef _SETUP_ISR_H_
#define _SETUP_ISR_H_

#include "DMAChannel.h"

#define NSAMPLES 1
#define DUMMY_DELTA (6*57)

// External variables; these are defined in setup_dma.cpp
extern volatile uint16_t pix_buffer[6*(NPIX+100)];
extern DMAChannel dma_adc_start; // DMA to start the ADC
extern DMAChannel dma_adc_stop;
extern DMAChannel dma_portc;


// Function declaration
void portd_isr();
void setup_shut_isr();

#endif
