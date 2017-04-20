#include "setup_clk.h"
#include "setup_dma.h"

// List of pins mapped to the ADC output
uint8_t const portc_pins[NBIT] = {15,22,23,9,10,13,11,12,28,27,29,30};

// ADC Mask
volatile uint8_t adc_stop = 0xFF;
volatile uint8_t adc_start = 0x00;

// External variables shared across multiple files
// Pixel buffer
volatile uint16_t pix_data[NPIX+100];
volatile uint16_t pix_buffer[NPIX+100];
volatile uint8_t pix_test[NPIX+100];

// DMA Channels
DMAChannel dma_portc; // DMA for reading PORTC
DMAChannel dma_adc_start; // DMA to start the ADC
DMAChannel dma_adc_stop; // DMA to stop the ADC
//DMAChannel dma_ftm0_stop; // DMA to stop the FTM0 clocks
//DMAChannel dma_ftm1_stop; // DMA to stop the FTM1 clocks

/* Function: setup_dma_portc
 * Description: sets the DMA request for transferring data from PORTC to the hold buffer 
 * Poll the register GPIOC_PDIR and transfer its content into the pixel of interest
 * Half of GPIOC_PDIR register is transferred (16 bits) since we are only about the first 12 bits
 */
void setup_dma_portc() {
    // Define all of our inputs
    for(int idx = 0; idx < NBIT; ++idx) {
        pinMode(portc_pins[idx],INPUT);
    }    

    // Enable DMA requests for FTM2, on the rising edge of port 32
    CORE_PIN32_CONFIG |= PORT_PCR_IRQC(2);

    // Use a single DMA channel triggered on the ADC to grab data from PORTC
    // Source
    dma_portc.source(GPIOC_PDIR);


    // Size
    dma_portc.transferSize(2); // 2 bytes = 16 bits
    dma_portc.transferCount(1); // Only one transfer 

    // Destination
    dma_portc.destinationBuffer(pix_buffer,2*(NPIX+100));

    // We increment destination by 2 bytes after every major loop count so that we can write next pixel in our buffer
    // The destination is reset in the ISR raised by SHUT falling down
//    dma_portc.TCD->DLASTSGA = 2;
    
    // Trigger on falling edge of FTM2
    // Since PORTB only has the ADC clock running, we can trigger on all of port B (how convenient is that?)
    // Having a DMA on the falling edge allows for data to be valid
    dma_portc.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    dma_portc.enable();

}

void reset_dma_portc() {
    dma_portc.disable();
    dma_portc.source(GPIOC_PDIR);
    dma_portc.destination(pix_buffer[0]);
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
    // Enable DMA requests on :
    // - rising edge of pin 24 (ROG)
    // - falling edge of pin 6 (SHUT)
    CORE_PIN24_CONFIG |= PORT_PCR_IRQC(1);  
    CORE_PIN6_CONFIG |= PORT_PCR_IRQC(2);

    dma_adc_start.source(adc_start);
    dma_adc_stop.source(adc_stop);

    dma_adc_start.destination(FTM2_OUTMASK);
    dma_adc_stop.destination(FTM2_OUTMASK);

    dma_adc_start.transferSize(1);
    dma_adc_stop.transferSize(1);

    dma_adc_start.transferCount(1);
    dma_adc_stop.transferCount(1);

    dma_adc_start.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
    dma_adc_stop.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTD);

    dma_adc_start.enable();
    dma_adc_stop.enable();
}

/* 
 * Function
 * Description: turn off all of the clocks
 *
 */
/*
void setup_dma_clock_stop() {
    dma_ftm0_stop.source(adc_start);
    dma_ftm1_stop.source(adc_start);

    dma_ftm0_stop.destination(FTM0_SC);
    dma_ftm1_stop.destination(FTM1_SC);

    dma_ftm0_stop.transferSize(1);
    dma_ftm1_stop.transferSize(1);

    dma_ftm0_stop.transferCount(1);
    dma_ftm1_stop.transferCount(1);

    dma_ftm0_stop.triggerAtCompletionOf(dma_adc_stop);
    dma_ftm1_stop.triggerAtCompletionOf(dma_adc_stop);

    dma_ftm0_stop.enable();
    dma_ftm1_stop.enable();
}
*/



/*
 * Function: setup_dma_buffer_transfer
 * Description: enables a DMA transfer that triggers when the ADC is shut down. 
 * The transfer moves data from our temporary buffer to the buffer to send to our host
 * Once the transfer is done, an ISR is raised so that serial operation can be done
 */
DMAChannel dma_buffer_transfer;
void setup_dma_buffer_transfer() {

//    for(int i = 0;i<NPIX+100;++i)
//        pix_buffer[i] = NPIX+100-i;

    dma_buffer_transfer.sourceBuffer(pix_buffer,2*(NPIX+100));
    dma_buffer_transfer.destinationBuffer(pix_data,2*(NPIX+100));

    dma_buffer_transfer.triggerAtCompletionOf(dma_adc_stop);
    dma_buffer_transfer.interruptAtCompletion();
    dma_buffer_transfer.attachInterrupt(setup_isr_buffer_transfer);

    dma_buffer_transfer.enable();
}

/* 
 * Function: setup_isr_buffer_transfer
 * Description: defines the tasks at hand when the data is staged for transfer to the host
 */
uint32_t tic,toc;

void setup_isr_buffer_transfer() {

//    uint32_t len = (NPIX+100)*sizeof(uint16_t);
//    for(int i = 0;i<NPIX+100;++i)
//
//        pix_data[i] = i%256;

//    uint32_t len = (NPIX+100)*sizeof(uint8_t);
//    for(int i = 0;i<NPIX+100;++i)
//        pix_data[i] = i;

    dma_buffer_transfer.clearInterrupt();
//    reset_dma_portc();
    //dma_buffer_transfer.detachInterrupt();

    tic = micros();
    Serial.flush();

    Serial.write(SOT,36); 
    Serial.write("\n");

    Serial.write((const uint8_t*)pix_data, 2*(NPIX+100));
    Serial.write("\n");
    Serial.send_now();

    toc = micros();
    toc -= tic;
    Serial.write((const uint8_t*)&toc,4);
    Serial.write("\n");
    Serial.send_now();

    //dma_buffer_transfer.disable();
}




/* 
 *  Function: setup_dma
 *  Description: shorthand to initialize everything
 *  
 */
void setup_dma() {
    setup_dma_portc();
    setup_dma_adc();
    setup_dma_buffer_transfer();

    Serial.print("DMA Channels\n");
    Serial.printf("PORTC: %d\n",dma_portc.channel);
    Serial.printf("ADC (start, stop): %d, %d\n",dma_adc_start.channel,dma_adc_stop.channel);
    Serial.printf("BUFFER TRANSFER: %d\n",dma_buffer_transfer.channel);
}

// See AN5083 https://cache.freescale.com/files/microcontrollers/doc/app_note/AN5083.pdf
