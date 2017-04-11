#include "setup_dma.h"

// Number of ADC bits
#define NBIT 12

// List of pins mapped to the ADC output
uint8_t portc_pins[NBIT] = {15,22,23,9,10,13,11,12,28,27,29,30};

void setup_dma_portc() {
    // Define all of our inputs
    for(int idx = 0; idx < NBIT; ++idx) {
        pinMode(portc_pins[NBIT],INPUT);
    }    

    // Use a single DMA channel triggered on the ADC to grab data from PORTC
    // Source
    dma_portc.source(GPIOC_PDIR);

    // Destination
    dma_portc.destination(pix_buffer[pix_num]);

    // Size
    dma_portc.transferSize(4); // 32 bits
    dma_portc.transferCount(1); // Only one transfer 
    
    // Trigger
    dma_portc.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
    dma_portc.enable();
}


// Function: dma_adc_setup
// Description: Start and stop the ADC clock with the DMA
// The source of the start and stop is a mask byte residing in memory
// and transferred to FTM2_OUTMASK
// Start of the ADC corresponds to ROG going low
// Stop of the ADC corresponds to SHUT going high 
void setup_dma_adc() {
    dma_adc_start.source(adc_start);
    dma_adc_stop.source(adc_stop);

    dma_adc_start.destination(FTM2_OUTMASK);
    dma_adc_stop.destination(FTM2_OUTMASK);

    dma_adc_start.transferSize(1);
    dma_adc_stop.transferSize(1);

    dma_adc_start.transferCount(1);
    dma_adc_stop.transferCount(1);

    dma_adc_start.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH2);
    dma_adc_stop.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH4);

    dma_adc_start.enable();
    dma_adc_stop.enable();
}

void setup_dma() {
    setup_dma_portc();
    setup_dma_adc();
}
