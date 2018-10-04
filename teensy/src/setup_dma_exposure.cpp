/* ILX526A Teensy Interface v1.0: a C interface running on the Teensy v3.2 to 
 * obtain data from an ILX526A CCD that is digitized with an ADS803U ADC.
 *
 * Copyright (C) 2018 Pierre-Yves Taunay
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact info: https://github.com/pytaunay
 *
 * Source: https://github.com/pytaunay/ILX526A
 */
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

