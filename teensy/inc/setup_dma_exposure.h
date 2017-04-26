#ifndef _SETUP_DMA_EXPOSURE_H_
#define _SETUP_DMA_EXPOSURE_H_


void setup_dma_reset_ftm1();
void setup_dma_reset_ftm0();
void setup_dma_exposure_cnt(); 
void setup_dma_exposure_cnt_start(); 
void isr_dma_exposure_cnt(); 


#endif
