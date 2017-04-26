#include "setup_clk.h"
#include "setup_dma.h"
#include "setup_isr.h"

#define __DEBUG__

uint32_t tic,toc;

// Boolean signaling data needs sent 
extern volatile uint8_t send_data;
extern volatile uint16_t pix_sum[2*(NPIX+100)];

uint8_t incoming_byte = 0;
uint8_t cmd_recvd = 0;
uint8_t cmd_buffer[16] = {0};
uint8_t cmd_idx = 0; 

extern "C" int main(void) {
    Serial.begin(9600);
    Serial.print("Test\n");
    // Setup
    setup_clk();
    delay(100);
    setup_dma();
//    setup_isr();

    Serial.print("End of setup\n");

    // Loop function
    while(1) {
        if(cmd_recvd) {
            switch(cmd_buffer[0]) {
                // We are changing the exposure
                case 'e':
                    //read_exposure();
                    //set_exposure();
                    break;


                // We are stopping _all_ operations
                case 'x':
                    break;

                // Single shot measurement
                case 's':
                    break;

                // Continuous measurement
                case 'c':
                    break;
            }
            // Ready for next command
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

//        if( send_data == 0x01 ) {
//
//            digitalWrite(15,HIGH);
//            tic = micros();
//            
//            for(int i = 0; i < NPIX+100;++i)
//                pix_data[i] = pix_sum[i] + pix_sum[i+NPIX+100];
//
//            Serial.flush();
//
//            // Send the start of transmission string
//            Serial.write(SOT,36); 
//            Serial.write("\n");
//
//            // Send the data 
//            Serial.write((const uint8_t*)pix_data, 2*(NPIX+100));
//            Serial.write("\n");
//
//            // Tell USB not to wait for chunks to be of size 64 bytes 
//            Serial.send_now();  
//
//            // Calculate the elapsed time in sending data, then send that information as well 
//            toc = micros();
//            toc -= tic;
//            Serial.write((const uint8_t*)&toc,4);
//            Serial.write("\n");
//            Serial.send_now();
//
//            send_data = 0x00;
//            digitalWrite(15,LOW);
//        }

    
    }

    return 0;
}    
