#include "setup_clk.h"
#include "setup_dma.h"
#include "setup_isr.h"
#include "utils.h"

#define __DEBUG__

uint32_t tic,toc;

// Boolean signaling data needs sent 
extern volatile uint8_t send_data;
extern volatile uint16_t pix_sum[2*(NPIX+100)];

uint8_t incoming_byte = 0;
uint8_t cmd_recvd = 0;
char cmd_buffer[16] = {0};
uint8_t cmd_idx = 0; 
uint32_t exposure = 240000-1; 

// Main loop
extern "C" int main(void) {
    // Initialize serial port at 9600 bauds
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
            Serial.printf("Send data!\n");

            digitalWrite(15,HIGH);
            tic = micros();
            
            for(int i = 0; i < NPIX+100;++i)
                pix_data[i] = pix_sum[i] + pix_sum[i+NPIX+100];

            Serial.flush();

            // Send the start of transmission string
            Serial.write(SOT,36); 
            Serial.write("\n");

            // Send the data 
            // Send 2*(NPIX+100) bytes one by one as opposed to (NPIX + 100) 2 bytes
            Serial.write((const uint8_t*)pix_data, 2*(NPIX+100));
            Serial.write("\n");

            // Tell USB not to wait for chunks to be of size 64 bytes 
            Serial.send_now();  

            // Calculate the elapsed time in sending data, then send that information as well 
            toc = micros();
            toc -= tic;
            Serial.write((const uint8_t*)&toc,4);
            Serial.write("\n");
            Serial.send_now();

            send_data = 0x00;
            digitalWrite(15,LOW);
        }
    }

    return 0;
}    
