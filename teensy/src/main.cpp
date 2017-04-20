#include "setup_clk.h"
#include "setup_dma.h"
#include "setup_isr.h"

#define __DEBUG__

extern "C" int main(void) {
    Serial.begin(9600);
    Serial.print("Test\n");
    // Setup
    setup_clk();
    setup_dma();
//    setup_isr();

    Serial.print("End of setup\n");

    // Loop function
    while(1) {}

    return 0;
}    
