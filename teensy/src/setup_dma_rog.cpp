#include "Arduino.h"
#include "DMAChannel.h"

// DMA Channels
DMAChannel dma_rog; // DMA to start the ADC
DMAChannel dma_enable_rog; // DMA to enable the ROG channel
extern DMAChannel dma_shut;


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
    dma_shut.enable();
    CORE_PIN5_CONFIG |= PORT_PCR_IRQC(2);

//    Serial.printf("DMA ROG interrupt - FTM1_OUTMASK: %d!\n",FTM1_OUTMASK);

}


// Mask to enable the ADC
// Only the 8 lowest bits of FTMx_OUTMASK are used.
// They correspond to each channel that use the timer.
// Setting the corresponding bit to 0 unmasks a clock.
// In this case, we unmask ALL FTM1 clocks (CCD and ADC)
// See section 36.3.13 in processor doc
volatile uint8_t adc_start = 0x00;

/* 
 *  Function: setup_dma_rog
 *  Description: Start and stop the ADC clock with the DMA
 *  The source of the start and stop is a mask byte residing in memory
 *  and transferred to FTM1_OUTMASK
 *  Start of the ADC corresponds to ROG going low
 *  Stop of the ADC corresponds to SHUT going high 
 *  
 */
void setup_dma_rog() {  
    // Enable DMA requests on :
    // - falling edge of pin 6 (ROG)
    CORE_PIN6_CONFIG |= PORT_PCR_IRQC(2);

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

