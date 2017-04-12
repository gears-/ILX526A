#ifndef _SETUP_ISR_H_
#define _SETUP_ISR_H_

#include "DMAChannel.h"

extern volatile uint16_t pix_buffer[6*(NPIX+100)];
extern float buffer_send[NPIX];
extern DMAChannel dma_adc_start; // DMA to start the ADC


volatile uint8_t nsamples_gathered;

void portd_isr();
void setup_shut_isr();

#endif
