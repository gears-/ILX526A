#include "setup_clk.h"
#include "setup_dma.h"
#include "setup_isr.h"

#define __DEBUG__

uint32_t tic,toc;

// Boolean signaling data needs sent 
extern volatile uint8_t send_data;
extern volatile uint16_t pix_sum[2*(NPIX+100)];

extern "C" int main(void) {
    Serial.begin(9600);
    Serial.print("Test\n");
    // Setup
    setup_clk();
    setup_dma();
//    setup_isr();

    Serial.print("End of setup\n");

    // Loop function
    while(1) {

        if( send_data == 0x01 ) {

            digitalWrite(15,HIGH);
            tic = micros();
            
            for(int i = 0; i < NPIX+100;++i)
                pix_data[i] = pix_sum[i] + pix_sum[i+NPIX+100];

            Serial.flush();

            // Send the start of transmission string
            Serial.write(SOT,36); 
            Serial.write("\n");

            // Send the data 
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
