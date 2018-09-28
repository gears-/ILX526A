#ifndef _SETUP_DMA_H_
#define _SETUP_DMA_H_

#include "Arduino.h"
#include "DMAChannel.h"

#include "setup_dma_exposure.h"
#include "setup_dma_ldval.h"
#include "setup_dma_buffer.h"
#include "setup_dma_rog.h"
#include "setup_dma_shut.h"

#include "setup_clk.h"

// Function declaration
void setup_dma();

// External variables shared across multiple files
// Pixel buffer
extern volatile uint16_t pix_data[BUF_SIZE];
extern volatile uint16_t pix_buffer[BUF_SIZE];


// DMA Channels
extern DMAChannel dma_exposure_cnt;
extern DMAChannel dma_exposure_cnt_start; // DMA for the exposure time

extern DMAChannel dma_rog;
extern DMAChannel dma_enable_rog; // DMA to enable the ROG channel
extern DMAChannel dma_shut;
extern DMAChannel dma_enable_shut; // DMA to enable the SHUT channel

extern DMAChannel dma_portc; // DMA for reading PORTC
extern DMAChannel dma_buffer_transfer;
extern DMAChannel dma_enable_send;




// Unique string generated with uuidgen for start of transmission
#define SOT "7eae261ddde24eeda293a7cd17e4379a"
#define EOT "476070cc-2bc0-498d-834d-3b9dfc6e79bf\n" 

#endif
