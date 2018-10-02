#include "Arduino.h"
#include "DMAChannel.h"

#include "setup_clk.h"

// DMA Channels
DMAChannel dma_shut; // DMA to stop the ADC
DMAChannel dma_enable_shut; // DMA to enable the SHUT channel
extern DMAChannel dma_rog;


/*
 * Function: isr_dma_shut
 * ISR raised by SHUT going low and disabling all the clocks
 * Takes care of disabling entirely the SHUT DMA, and re-arms the ROG DMA
 *
 */
void isr_dma_shut() {
    dma_shut.clearInterrupt();

    // Set the ADC clock low for next cycle
    CORE_PIN17_CONFIG &= ~PORT_PCR_MUX(3); 
    CORE_PIN17_CONFIG |= PORT_PCR_MUX(1);
    GPIOB_PDOR = 0;

    // Disable SHUT
    volatile uint8_t *mux;
    mux = (volatile uint8_t *)&(DMAMUX0_CHCFG0) + dma_shut.channel;
    *mux &= ~DMAMUX_ENABLE;
    dma_shut.disable();
    CORE_PIN5_CONFIG &= ~PORT_PCR_IRQC(0);

    // Enable ROG
    mux = (volatile uint8_t *)&(DMAMUX0_CHCFG0) + dma_rog.channel;
    *mux |= DMAMUX_ENABLE;
    dma_rog.enable();
    CORE_PIN6_CONFIG |= PORT_PCR_IRQC(2);

    // Re-initializes the source of the DMA from PORTC
    // TODO
}

/*
 * Function: setup_dma_shut
 * Description: trigger for a cascade of events when the shutter drain closes
 * The following events occur:
 * - ADC gets disabled by setting the second bit of FTM1_OUTMASK to 1
 * - Exposure starts with a PIT timer  
 * - Data gets transferred through USB by setting a flag which is picked up
 *   by the main program loop 
 */
uint32_t disable_clocks = (SIM_SCGC6 & ~SIM_SCGC6_FTM0 & ~SIM_SCGC6_FTM1) | SIM_SCGC6_DMAMUX | SIM_SCGC6_PIT | SIM_SCGC6_ADC0 | SIM_SCGC6_RTC | SIM_SCGC6_FTFL ; 
uint8_t adc_stp = 0x02;
void setup_dma_shut() {
    // Enable DMA requests on
    // - falling edge of pin 5 (SHUT)
//    CORE_PIN5_CONFIG |= PORT_PCR_IRQC(2);

    dma_shut.source(disable_clocks);
    dma_shut.destination(SIM_SCGC6);

    // Transfer the whole register -- 32 bits = 4 bytes
    dma_shut.transferSize(4);
    dma_shut.transferCount(1);

    dma_shut.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTD);

    dma_shut.interruptAtCompletion();
    dma_shut.attachInterrupt(isr_dma_shut);

    dma_shut.disableOnCompletion();
    // This DMA is enabled by the ROG DMA
}

