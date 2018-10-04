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
#include "setup_dma.h"
/* 
 *  Function: setup_dma
 *  Description: shorthand to initialize everything
 *  
 */
void setup_dma() {
    // Exposure
    setup_dma_exposure_cnt_start(); 

    // PIT0 length (LDVAL)
    setup_dma_ldval();

    // Transfer data from PORTC
    setup_dma_portc();

    // Buffer manipulation
    setup_dma_buffer_transfer();

    // ROG
    setup_dma_rog();

    // SHUT
    setup_dma_shut();

    // Print setup information
    Serial.print("DMA Channels\n");
    Serial.printf("Exposure control: PIT0 countdown %d\t PIT0 starter %d\n",dma_exposure_cnt.channel,dma_exposure_cnt_start.channel);
    Serial.printf("ROG: ADC start %d\t Enabling ROG %d\n",dma_rog.channel,dma_enable_rog.channel);
    Serial.printf("SHUT: ADC start %d\t Enabling SHUT %d\n",dma_shut.channel,dma_enable_shut.channel);
    Serial.printf("PORTC: %d\n",dma_portc.channel);
    Serial.printf("BUFFER TRANSFER: %d\n",dma_buffer_transfer.channel);
}

// See AN5083 https://cache.freescale.com/files/microcontrollers/doc/app_note/AN5083.pdf
