#include "DMAChannel.h"
#include "Arduino.h"
#include "setup_dma.h"

// Number of ADC bits
#define NBIT 12

// List of pins mapped to the ADC output
uint8_t portc_pins[NBIT] = {15,22,23,9,10,13,11,12,28,27,29,30};

// ADC Mask
static uint8_t adc_stop = 0xFF;
static uint8_t adc_start = 0x00;


/* Function: setup_dma_portc
 * Description: sets the DMA request for transferring data from PORTC to the hold buffer 
 * Poll the register GPIOC_PDIR and transfer its content into the pixel of interest
 * Half of GPIOC_PDIR register is transferred (16 bits) since we are only about the first 12 bits
 */
void setup_dma_portc() {
    // Define all of our inputs
    for(int idx = 0; idx < NBIT; ++idx) {
        pinMode(portc_pins[NBIT],INPUT);
    }    

    // Enable DMA requests for FTM2, on the falling edge of port 32
    CORE_PIN32_CONFIG |= PORT_PCR_IRQC(2);

    // Use a single DMA channel triggered on the ADC to grab data from PORTC
    // Source
    dma_portc.source(GPIOC_PDIR);

    // Destination
    dma_portc.destination(pix_buffer[0]);

    // Size
    dma_portc.transferSize(2); // 2 bytes = 16 bits
    dma_portc.transferCount(1); // Only one transfer 

    // We increment destination by 2 bytes after every major loop count so that we can write next pixel in our buffer
    // The destination is reset in the ISR raised by SHUT falling down
    dma_portc.TCD->DLASTSGA = 2;
    
    // Trigger on falling edge of FTM2
    // Since PORTB only has the ADC clock running, we can trigger on all of port B (how convenient is that?)
    // Having a DMA on the falling edge allows for data to be valid
    dma_portc.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    dma_portc.enable();
}


/* 
 *  Function: dma_adc_setup
 *  Description: Start and stop the ADC clock with the DMA
 *  The source of the start and stop is a mask byte residing in memory
 *  and transferred to FTM2_OUTMASK
 *  Start of the ADC corresponds to ROG going low
 *  Stop of the ADC corresponds to SHUT going high 
 *  
 */
void setup_dma_adc() {  
    // Enable DMA requests on falling edge of pin 24 (ROG)
    CORE_PIN24_CONFIG |= PORT_PCR_IRQC(2);  

    dma_adc_start.source(adc_start);
//    dma_adc_stop.source(adc_stop);

    dma_adc_start.destination(FTM2_OUTMASK);
//    dma_adc_stop.destination(FTM2_OUTMASK);

    dma_adc_start.transferSize(1);
//    dma_adc_stop.transferSize(1);

    dma_adc_start.transferCount(1);
//    dma_adc_stop.transferCount(1);

    dma_adc_start.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH4);
//    dma_adc_stop.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH2);

    dma_adc_start.enable();
//    dma_adc_stop.enable();
}


/* 
 *  Function: setup_dma
 *  Description: shorthand to initialize everything
 *  
 */
void setup_dma() {
    setup_dma_portc();
    setup_dma_adc();
}
