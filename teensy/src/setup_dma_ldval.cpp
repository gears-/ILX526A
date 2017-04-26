#include "DMAChannel.h"

DMAChannel dma_ldval;
extern DMAChannel dma_rog;
extern uint32_t exposure;

/*
 * Function: setup_dma_ldval
 * Description: sets the DMA request for updating PIT_LDVAL0
 * Once ROG starts the ADC, then the counter for PIT0 is updated
 */
void setup_dma_ldval() {
    dma_ldval.source(exposure);
    dma_ldval.destination(PIT_LDVAL0);

    dma_ldval.transferSize(4);
    dma_ldval.transferCount(1);

    dma_ldval.triggerAtCompletionOf(dma_rog);

    dma_ldval.enable();
}
