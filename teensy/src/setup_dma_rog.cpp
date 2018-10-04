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
#include "Arduino.h"
#include "DMAChannel.h"

// DMA Channels
DMAChannel dma_rog; // DMA to start the ADC
DMAChannel dma_enable_rog; // DMA to enable the ROG channel
//DMAChannel dma_enable_portc_transfer; // DMA to enable the transfers from PORTC to the buffer
extern DMAChannel dma_shut;
extern DMAChannel dma_mask_ftm1;
extern DMAChannel dma_portc;


/*
 * Function: isr_dma_rog
 * Description: ISR raised by starting the ADC
 * The ISR completely disables the ROG DMA, and arms the SHUT DMA
 *
 */
void isr_dma_rog() {
    dma_rog.clearInterrupt();

    volatile uint8_t *mux;

    // Disable ROG
    mux = (volatile uint8_t *)&(DMAMUX0_CHCFG0) + dma_rog.channel;
    *mux &= ~DMAMUX_ENABLE;
    dma_rog.disable();
    CORE_PIN6_CONFIG &= ~PORT_PCR_IRQC(0);

    // Enable SHUT
    mux = (volatile uint8_t *)&(DMAMUX0_CHCFG0) + dma_shut.channel;
    *mux |= DMAMUX_ENABLE;
    dma_mask_ftm1.enable();
    dma_shut.enable();
    CORE_PIN5_CONFIG |= PORT_PCR_IRQC(2);
}


// Mask to enable the ADC
// Only the 8 lowest bits of FTMx_OUTMASK are used.
// They correspond to each channel that use the timer.
// Setting the corresponding bit to 0 unmasks a clock.
// In this case, we unmask ALL FTM1 clocks (CCD and ADC)
// Note that the CCD clock was already unmasked by the PIT0 ISR
// See section 36.3.13 in processor doc
const uint8_t adc_start = 0x00;

/* 
 *  Function: setup_dma_rog
 *  Description: Starts with a DMA request
 *  The source of the start and stop is a mask byte residing in memory
 *  and transferred to FTM1_OUTMASK
 *  Start of the ADC corresponds to ROG going low
 *  Stop of the ADC corresponds to SHUT going high 
 *  
 */
void setup_dma_rog() {  
    //// Enable DMA requests on falling edge of pin 6 (ROG)
    CORE_PIN6_CONFIG |= PORT_PCR_IRQC(2);

    //// DMA to unmask the ADC, chained from the PORTC one
    dma_rog.source(adc_start);
    dma_rog.destination(FTM1_OUTMASK);

    dma_rog.transferSize(1);
    dma_rog.transferCount(1);

    dma_rog.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTD);
    dma_rog.disableOnCompletion();

    dma_rog.interruptAtCompletion();
    dma_rog.attachInterrupt(isr_dma_rog);

    dma_rog.enable();
}

