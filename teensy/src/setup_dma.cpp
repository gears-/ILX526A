#include "setup_clk.h"
#include "setup_dma.h"

// List of pins mapped to the ADC output
uint8_t const portc_pins[NBIT] = {15,22,23,9,10,13,11,12,28,27,29,30};

// ADC Mask
volatile uint8_t adc_stop = 0xFF;
volatile uint8_t adc_start = 0x00;

// DMA Channels
DMAChannel dma_portc; // DMA for reading PORTC
DMAChannel dma_adc_start; // DMA to start the ADC
DMAChannel dma_adc_stop; // DMA to stop the ADC

volatile uint8_t send_data = 0x00;
volatile uint16_t pix_data[NPIX+100] = {0};
volatile uint16_t pix_buffer[NPIX+100] = {0};
volatile uint16_t pix_sum[2*(NPIX+100)] = {0};

/* Function: setup_dma_portc
 * Description: sets the DMA request for transferring data from PORTC to the hold buffer 
 * Poll the register GPIOC_PDIR and transfer its content into the pixel of interest
 * Half of GPIOC_PDIR register is transferred (16 bits) since we are only about the first 12 bits
 */
void setup_dma_portc() {
    // Define all of our inputs
    for(int idx = 0; idx < NBIT; ++idx) {
        pinMode(portc_pins[idx],OUTPUT);
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
//    dma_portc.enable();

}

//void reset_dma_portc() {
//    dma_portc.disable();
//    dma_portc.source(GPIOC_PDIR);
 //   dma_portc.destination(pix_buffer[0]);
//    dma_portc.enable();
//}    


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
 * The transfer moves the raw data gathered into pix_buffer into a staging buffer.
 * The staging buffer is twice the size of pix_buffer so that it can average two complete samples.
 * Once the transfer is done, an ISR is raised so that averaging can be triggered, and data can be sent away. 
 */
DMAChannel dma_buffer_transfer;
DMAChannel dma_enable_send;
uint8_t trig_send_data = 0x01;
void setup_dma_buffer_transfer() {

    for(int i = 0;i<NPIX+100;++i) {
        pix_buffer[i] = NPIX+100-i;
        pix_data[i] = 0;
    }

    for(int i = 0;i<2*(NPIX+100);++i)
        pix_sum[i] = 0;

    dma_buffer_transfer.source(pix_buffer[0]);
    dma_buffer_transfer.destination(pix_sum[0]);
    //dma_buffer_transfer.destination(pix_data[0]);

    // Set up 2 transfers of uint16_t array of size NPIX+100
    // Corresponds to 2*2*(NPIX+100) bytes
    dma_buffer_transfer.TCD->NBYTES = 2*(NPIX+100);
    dma_buffer_transfer.TCD->CITER  = 2;
    dma_buffer_transfer.TCD->BITER  = 2;
    dma_buffer_transfer.TCD->SOFF = 2;
    dma_buffer_transfer.TCD->DOFF = 2;
    dma_buffer_transfer.TCD->ATTR_SRC = 1; 
    dma_buffer_transfer.TCD->ATTR_DST = 1; 
    dma_buffer_transfer.TCD->SLAST = -2*(NPIX+100);
    dma_buffer_transfer.TCD->DLASTSGA = -4*(NPIX+100);

    // The DMA triggers when the ADC stops 
    dma_buffer_transfer.triggerAtCompletionOf(dma_adc_stop);

    // Enables an interrupt at half to reset the source address...
    // There may be a more elegant way to do that
    dma_buffer_transfer.interruptAtHalf();
    dma_buffer_transfer.attachInterrupt(isr_buffer_transfer_src_reset);

    dma_buffer_transfer.enable();

    dma_enable_send.source(trig_send_data);
    dma_enable_send.destination(send_data);
    dma_enable_send.transferSize(1);
    dma_enable_send.transferCount(1);
    dma_enable_send.triggerAtCompletionOf(dma_buffer_transfer);

//    dma_enable_send.interruptAtCompletion();
//    dma_enable_send.attachInterrupt(isr_buffer_transfer);

    dma_enable_send.enable();
}

void isr_buffer_transfer_src_reset() {
    dma_buffer_transfer.clearInterrupt();
    dma_buffer_transfer.TCD->SADDR = &pix_buffer[0];
}

//void isr_buffer_transfer() {
//    dma_enable_send.clearInterrupt();
//    Serial.print("ISR from DMA\n");
//}


/* 
 * Function: setup_isr_buffer_transfer
 * Description: clears the interrupt raised by the DMA transfer, then sets a flag that tells the main program to send data away 
 */
//void isr_buffer_transfer() {
//    dma_buffer_transfer.clearInterrupt();
//    send_data = true;
//}




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
