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

/* 
 *  Function: setup_dma_rog
 *  Description: Start and stop the ADC clock with the DMA
 *  The source of the start and stop is a mask byte residing in memory
 *  and transferred to FTM1_OUTMASK
 *  Start of the ADC corresponds to ROG going low
 *  Stop of the ADC corresponds to SHUT going high 
 *  
 */
volatile uint8_t adc_start = 0x00;
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




/*
void isr_dma_enable_rog() {
    dma_enable_rog.clearInterrupt();

    Serial.print("DMA enable ROG interrupt!\n");
}
*/

/*
 * Function: setup_dma_enable_rog
 * Description: arm the ROG DMA by transferring the channel number into DMA_SERQ
 * DMA_SERQ sets the "Enable Request Register" (DMA_ERQ)
 * This approach is more reliable than an ISR
 *
void setup_dma_enable_rog() {
    dma_enable_rog.source(dma_rog.channel);
    dma_enable_rog.destination(DMA_SERQ);

    dma_enable_rog.transferSize(1);
    dma_enable_rog.transferCount(1);

//    dma_enable_rog.triggerAtCompletionOf(dma_shut);

//    dma_enable_rog.interruptAtCompletion();
//    dma_enable_rog.attachInterrupt(isr_dma_enable_rog);

//    dma_enable_rog.enable();
}
*/ 
