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

void setup_dma_buffer_transfer();
void isr_buffer_transfer();
void isr_buffer_transfer_src_reset(); 



// External variables shared across multiple files
// Pixel buffer
extern volatile uint16_t pix_data[NPIX+100];
extern volatile uint16_t pix_buffer[NPIX+100];


// Unique string generated with uuidgen for start of transmission
#define SOT "7eae261d-dde2-4eed-a293-a7cd17e4379a"
#define EOT "476070cc-2bc0-498d-834d-3b9dfc6e79bf\n" 

#endif
