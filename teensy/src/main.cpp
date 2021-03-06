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
#include "setup_clk.h"
#include "setup_dma.h"
#include "setup_isr.h"
#include "utils.h"

// Boolean signaling data needs sent 
extern volatile uint8_t send_data;
extern volatile uint16_t pix_sum[2*(NPIX+100)];

uint8_t incoming_byte = 0;
uint8_t cmd_recvd = 0;
char cmd_buffer[16] = {0};
uint8_t cmd_idx = 0; 
uint32_t exposure = 240000-1; 

char ret = '\n';
uint32_t start_frame = (uint32_t)SOT; 

uint8_t blank_frame[40] = {0};

// Main loop
extern "C" int main(void) {
    // Set the pin 26 as an output for data indication
    pinMode(26,OUTPUT);

    // Initialize serial port at 9600 bauds (actual number does not matter; USB always work full speed)
    Serial.begin(9600);
    // Setup clock and DMA requests
    setup_clk();
    delay(100);
    setup_dma();

    Serial.print("End of setup\n");

    // Loop function
    while(1) {
        // Structure to treat the commands received
        if(cmd_recvd) {
            // Parse the command
            // Return an error if it is the wrong command
            CMD_PARSE_RET ret = cmd_parse(cmd_buffer);
            
            switch(ret){
                case CMD_PARSE_RET::EXPOSURE_MATCH:
                    exposure = read_exposure(cmd_buffer);
                    // Serial.printf("LDVAL required is: %d\n",exposure);
                    break;

                case CMD_PARSE_RET::NO_MATCH:
                    Serial.printf("I do not recognize this command: %s\n", cmd_buffer);
                    break;
            }

            // Ready for next command: reset cmd_idx, cmd_recvd, and the buffer that holds
            // the command
            cmd_idx = 0;
            cmd_recvd = 0;
            memset(cmd_buffer,0,sizeof(cmd_buffer));
        } 

        // Did the host send us something?
        if ( Serial.available()) {
            incoming_byte = Serial.read();

            // If our incoming byte is linefeed, this is the end of the command
            if( incoming_byte == 0x0a ) {
                cmd_buffer[cmd_idx++] = '\0';
                cmd_recvd = 1;
            } 
            // Otherwise it's a command instruction
            else {
               cmd_buffer[cmd_idx++] = incoming_byte;
              cmd_recvd = 0; 
            }
        }

        // Should we send data?
        if( send_data == 0x01 ) {
#ifdef __DEBUG__
            digitalWrite(26,HIGH);
#endif
            //for(int i = 0; i < NPIX+100;++i) {
            //    pix_data[i] = pix_buffer[i];
            //    pix_buffer[i] = 0;
            //}

            //// DATA FRAME
            // Send the data cast a uint8_t since Serial.write does not work w/ uint16_t types 
            Serial.write((const uint8_t*)start_frame,32); // Start of transmission string: 32 bytes
            Serial.write((const uint8_t*)pix_sum,2*(NPIX+100)); // Actual data: 6200 bytes (3100 uint16_t)
            Serial.write((const uint8_t*)blank_frame,40); // Padding: 40 bytes since we complete our 98 x 64 bytes packet with an additional write 
            // A USB packet on the Teensy is sent if of size 64 bytes. Optimizes the transmission.

#ifdef __DEBUG__
            digitalWrite(26,LOW);
#endif
            send_data = 0x00;
        }
    }

    return 0;
}    
