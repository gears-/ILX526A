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
