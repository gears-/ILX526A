#include "setup_clk.h"
#include "setup_dma.h"

extern "C" int main(void) {

    // Setup
    setup_clk();
    setup_dma();


    // Loop function
    while(1) {
    }

    return 0;
}    
