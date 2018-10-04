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
#include "DMAChannel.h"

DMAChannel dma_ldval;
extern DMAChannel dma_rog;
extern uint32_t exposure;

/*
 * Function: setup_dma_ldval
 * Description: sets the DMA request for updating PIT_LDVAL0
 * Once ROG starts the ADC, then the counter for PIT0 is updated
 * PIT_LDVALn is a 32 bit number. 
 * See Section 37.3.2 in the processor doc
 */
void setup_dma_ldval() {
    dma_ldval.source(exposure);
    dma_ldval.destination(PIT_LDVAL0);

    dma_ldval.transferSize(4); // Transfer 4 bytes (32 bits)
    dma_ldval.transferCount(1);

    dma_ldval.triggerAtCompletionOf(dma_rog);

    dma_ldval.enable();
}
