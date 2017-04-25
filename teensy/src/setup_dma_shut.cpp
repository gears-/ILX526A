#include "Arduino.h"
#include "DMAChannel.h"

#include "setup_clk.h"

// DMA Channels
DMAChannel dma_shut; // DMA to stop the ADC
DMAChannel dma_enable_shut; // DMA to enable the SHUT channel
extern DMAChannel dma_rog;

volatile uint8_t adc_stop = ADC_MASK;


void isr_dma_shut() {
    dma_shut.clearInterrupt();
    Serial.print("DMA SHUT Interrupt!\n");

    //SIM_SCGC6 &= ~SIM_SCGC6_FTM0 & ~SIM_SCGC6_FTM1;
}

/*
 * Function: setup_dma_enable_shut
 * Description: arm the SHUT DMA by transferring the channel number into DMA_SERQ
 * DMA_SERQ sets the "Enable Request Register" (DMA_ERQ)
 * This approach is more reliable than an ISR
 *
 */ 
void setup_dma_enable_shut() {
    dma_enable_shut.source(dma_shut.channel);
    dma_enable_shut.destination(DMA_SERQ);

    dma_enable_shut.transferSize(1);
    dma_enable_shut.transferCount(1);

    dma_enable_shut.triggerAtCompletionOf(dma_rog);

    dma_enable_shut.enable();
}

/*
 *
 * Function: setup_dma_shut
 * Description: trigger for a cascade of events when the shutter drain closes
 * The following events occur:
 * - ADC gets disabled
 * - Exposure starts with a PIT timer  
 * - Data gets transferred through USB by setting a flag which is picked up
 *   by the main program loop 
 */
uint32_t disable_clocks = SIM_SCGC6 & ~SIM_SCGC6_FTM0 & ~SIM_SCGC6_FTM1; 
void setup_dma_shut() {
    // Enable DMA requests on
    // - falling edge of pin 5 (SHUT)
    CORE_PIN5_CONFIG |= PORT_PCR_IRQC(2);

    dma_shut.source(disable_clocks);
    dma_shut.destination(SIM_SCGC6);

    // Transfer the whole register -- 32 bits = 4 bytes
    dma_shut.transferSize(4);
    dma_shut.transferCount(1);

    dma_shut.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTD);

    dma_shut.interruptAtCompletion();
    dma_shut.attachInterrupt(isr_dma_shut);

    dma_shut.disableOnCompletion();

}

