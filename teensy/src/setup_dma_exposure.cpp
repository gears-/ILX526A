#include "Arduino.h"
#include "DMAChannel.h"

#include "setup_dma_exposure.h"
#include "setup_clk.h"

// DMA Channels
DMAChannel dma_exposure_cnt;
DMAChannel dma_exposure_cnt_start; // DMA for the exposure time
extern DMAChannel dma_enable_rog;
extern DMAChannel dma_shut;

// Mask to start the PIT timer for exposure
// Controls directly the "Timer control register" PIT_TCTRLn where n = 0
// See Section 37.3.4 of the processor doc
// PIT_TCTRL_TEN: Timer enable - defined in kinetis.h as (1<<0) (i.e. 0x01) 
// PIT_TCTRL_TIE: Timer interrupt enable - defined in kinetis.h as (1<<1) (i.e. 0x02)
uint8_t cnt_start = PIT_TCTRL_TEN | PIT_TCTRL_TIE;

/*
 * Function: setup_dma_exposure_cnt_start
 * Description: sets a DMA that enables the PIT timer for exposure 
 * It is triggered when the dma_shut finishes stopping the ADC clock
 *
 */
void setup_dma_exposure_cnt_start() {
    dma_exposure_cnt_start.source(cnt_start);
    dma_exposure_cnt_start.destination(PIT_TCTRL0);
    dma_exposure_cnt_start.transferSize(1);
    dma_exposure_cnt_start.transferCount(1);

    dma_exposure_cnt_start.triggerAtCompletionOf(dma_shut);

    dma_exposure_cnt_start.enable();
}

