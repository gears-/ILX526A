#include "Arduino.h"
#include "DMAChannel.h"

#include "setup_dma_exposure.h"
#include "setup_clk.h"

// DMA Channels
DMAChannel dma_exposure_cnt;
DMAChannel dma_reset_ftm1;
DMAChannel dma_reset_ftm0;
DMAChannel dma_exposure_cnt_start; // DMA for the exposure time
extern DMAChannel dma_enable_rog;
extern DMAChannel dma_shut;


void isr_reset_ftm1() {
    dma_reset_ftm1.clearInterrupt();
}


uint8_t reset_ftm = 0x00;
void setup_dma_reset_ftm1() {
    dma_reset_ftm1.source(reset_ftm);
    dma_reset_ftm1.destination(FTM1_CNT);

    dma_reset_ftm1.transferSize(1);
    dma_reset_ftm1.transferCount(1);

    dma_reset_ftm1.triggerAtCompletionOf(dma_exposure_cnt);

    dma_reset_ftm1.interruptAtCompletion();
    dma_reset_ftm1.attachInterrupt(isr_reset_ftm1);

    dma_reset_ftm1.enable();

}


void setup_dma_reset_ftm0() {
    dma_reset_ftm0.source(reset_ftm);
    dma_reset_ftm0.destination(FTM0_CNT);

    dma_reset_ftm0.transferSize(1);
    dma_reset_ftm0.transferCount(1);

    dma_reset_ftm0.triggerAtCompletionOf(dma_reset_ftm1);

    dma_reset_ftm0.enable();

}

/*
 * Function: setup_dma_exposure_cnt
 * Descriptino: 
 *
 */
uint8_t dummy = 0x00;
uint32_t enable_clocks = SIM_SCGC6 | SIM_SCGC6_FTM0  |SIM_SCGC6_FTM1 | SIM_SCGC6_DMAMUX | SIM_SCGC6_PIT | SIM_SCGC6_ADC0 | SIM_SCGC6_RTC | SIM_SCGC6_FTFL; 
void setup_dma_exposure_cnt() {
    dma_exposure_cnt.source(enable_clocks);
    dma_exposure_cnt.destination(SIM_SCGC6);

    dma_exposure_cnt.transferSize(4);
    dma_exposure_cnt.transferCount(1);

    DMAMUX0_CHCFG0 |= DMAMUX_ENABLE | DMAMUX_TRIG | DMAMUX_SOURCE_ALWAYS0;
//    DMAMUX0_CHCFG0 |= DMAMUX_ENABLE | DMAMUX_TRIG | DMAMUX_SOURCE_PORTD;

    dma_exposure_cnt.interruptAtCompletion();
    dma_exposure_cnt.attachInterrupt(isr_dma_exposure_cnt);
    dma_exposure_cnt.enable();
}


void isr_dma_exposure_cnt_start() {
    dma_exposure_cnt_start.clearInterrupt();
//    Serial.print("Exposure PIT TCTRL0 interrupt!\n");
}

/*
 * Function: setup_dma_exposure_cnt_start
 * Description: sets a DMA that enables the PIT timer for exposure 
 * It is triggered when the dma_shut finishes stopping the ADC clock
 *
 */
uint8_t cnt_start = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
void setup_dma_exposure_cnt_start() {
    dma_exposure_cnt_start.source(cnt_start);
    dma_exposure_cnt_start.destination(PIT_TCTRL0);
    dma_exposure_cnt_start.transferSize(1);
    dma_exposure_cnt_start.transferCount(1);

    dma_exposure_cnt_start.triggerAtCompletionOf(dma_shut);
//    dma_exposure_cnt_start.interruptAtCompletion();
//    dma_exposure_cnt_start.attachInterrupt(isr_dma_exposure_cnt_start);

    dma_exposure_cnt_start.enable();
}

/* 
 * Function: isr_dma_exposure_cnt()
 * Description: interrupt service routine once the PIT0 timer has been exhausted
 * Roles:
 * - Restarts the FTM clocks
 * - Resets the "always enabled" DMA channel based on the kinetis K20 errata
 * (see document 15823 - "Mask Set Errata for Mask 4N22D")
 * (http://www.nxp.com/docs/pcn_attachments/15823_KINETIS_4N22D.pdf)
 *
 * Excerpt:
 * If using the PIT to trigger a DMA channel where the major loop count is set to one, then in
 * order to get the desired periodic triggering, the DMA must do the following in the interrupt
 * service routine for the DMA_DONE interrupt:
 * 1. Set the DMA_TCDn_CSR[DREQ] bit and configure DMAMUX_CHCFGn[ENBL] = 0
 * 2. Then again DMAMUX_CHCFGn[ENBL] = 1, DMASREQ=channel in your DMA DONE
 * interrupt service routine so that "always enabled" source could negate its request then DMA
 * request could be negated.
 * This will allow the desired periodic triggering to function as expected.
 *
 */
void isr_dma_exposure_cnt() {
    dma_exposure_cnt.clearInterrupt();
    // Clear it
    PIT_TFLG0 = 1;
//    Serial.print("DMA Exposure CNT\n");

    DMAMUX0_CHCFG0 &= ~DMAMUX_ENABLE;
    DMAMUX0_CHCFG0 |= DMAMUX_ENABLE;

    // Disable the PIT timer
//    PIT_TCTRL0 &= ~PIT_TCTRL_TEN;

    // Restart the clocks in one fell swoop
//    SIM_SCGC6 |= SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1; 
//    FTM1_CNT = 0;
//    FTM0_CNT = 0;
}


