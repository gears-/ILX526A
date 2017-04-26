#include "setup_dma.h"
/* 
 *  Function: setup_dma
 *  Description: shorthand to initialize everything
 *  
 */
void setup_dma() {

    // Exposure
    setup_dma_exposure_cnt(); 
    setup_dma_reset_ftm1();
    setup_dma_reset_ftm0();
    setup_dma_exposure_cnt_start(); 

    // Buffer manipulation
//    setup_dma_portc();
//    setup_dma_buffer_transfer();

    // ROG
    setup_dma_rog();
//    setup_dma_enable_shut();

    // SHUT
    setup_dma_shut();
//    setup_dma_enable_rog();


    // Print setup information
    Serial.print("DMA Channels\n");
    Serial.printf("Exposure control: PIT0 countdown %d\t PIT0 starter %d\n",dma_exposure_cnt.channel,dma_exposure_cnt_start.channel);
    Serial.printf("ROG: ADC start %d\t Enabling ROG %d\n",dma_rog.channel,dma_enable_rog.channel);
    Serial.printf("SHUT: ADC start %d\t Enabling SHUT %d\n",dma_shut.channel,dma_enable_shut.channel);
    Serial.printf("PORTC: %d\n",dma_portc.channel);
    Serial.printf("BUFFER TRANSFER: %d\n",dma_buffer_transfer.channel);
}

// See AN5083 https://cache.freescale.com/files/microcontrollers/doc/app_note/AN5083.pdf
