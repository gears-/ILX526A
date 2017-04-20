#ifndef _SETUP_ISR_H_
#define _SETUP_ISR_H_

#include "DMAChannel.h"

#define NSAMPLES 1
#define DUMMY_DELTA (6*57)

// External variables; these are defined in setup_dma.cpp
extern volatile uint16_t pix_buffer[NPIX+100];
extern volatile uint16_t pix_data[NPIX+100];

// Function declaration
void setup_isr();

#endif
