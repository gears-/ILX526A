#include "Arduino.h"
#include "setup_clk.h"

void ccd_clk() {
    // Halt timers
    FTM1_SC = 0;

    // Count at which switch occurs
    FTM1_C0V = 30;

    // See table p783
    // This config yields edge aligned PWM with high-true pulses
    // DECAPEN = 0
    // COMBINE = 0
    // CPWMS = 0
    // MSNB, MSNA = 10 -> edge aligned PWM
    // ELSB, ELSA = 10 -> high-true pulses
    FTM1_C0SC = FTM_CSC_MSB | FTM_CSC_ELSA;    
    FTM1_C0SC |= FTM_CSC_DMA | FTM_CSC_CHIE;

    // Set initial count
    FTM1_CNTIN = 0;

    // Enable global time base slave
    FTM1_CONF |= FTM_CONF_GTBEEN & ~FTM_CONF_GTBEOUT;

    // Start clocks
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);

    // Reset CNT register
    FTM1_CNT = 0;

    // Timer period
    FTM1_MOD = 59;
    
    // Pin configuration - no slew rate
    CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE;

}

void adc_clk() {
    // Halt timers
    FTM2_SC = 0;

    // Count at which switch occurs
    //FTM2_C0V = 5;
    FTM2_C0V = 30;

    // See table p783
    // This config yields edge aligned PWM with high-true pulses
    // DECAPEN = 0
    // COMBINE = 0
    // CPWMS = 0
    // MSNB, MSNA = 10 -> edge aligned PWM
    // ELSB, ELSA = 10 -> high-true pulses
    FTM2_C0SC = FTM_CSC_MSB | FTM_CSC_ELSB;    

    // Enable DMA requests from FTM2
    FTM2_C0SC |= FTM_CSC_DMA | FTM_CSC_CHIE;

    // Set initial counter
    FTM2_CNTIN = 0;

    // Enable global time base slave
    FTM2_CONF |= FTM_CONF_GTBEEN & ~FTM_CONF_GTBEOUT;

    // Start clocks
    FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);

    // Reset CNT register
    FTM2_CNT = 0;

    // Timer period
    //FTM2_MOD = 9;
    FTM2_MOD = 59;
    
    // Pin configuration - no slew rate
    CORE_PIN32_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE;

    // We mask the ADC clock until it is triggered
    FTM2_OUTMASK = 0xFF;
}


void pwm_clk() {
    // Halt timers
    FTM0_SC = 0;

    // Count that corresponds to when switch occurs
    FTM0_C2V = 2;
    FTM0_C3V = 152;

    FTM0_C4V = 46013;
    FTM0_C5V = 46013+60;

    // ROG, SHUT
    // See table p783
    // This config yields high true pulses on channel n match
    // DECAPEN = 0
    // COMBINE = 1
    // CPWMS = 0 
    // MSnB:MSnA = xx
    // ELSnB:ELSnA = 10
    FTM0_COMBINE = FTM_COMBINE_COMBINE1 | FTM_COMBINE_COMBINE2; // Combine channels 2 and 3, and 4 and 5
    FTM0_C2SC |= FTM_CSC_ELSB;
    FTM0_C4SC |= FTM_CSC_ELSB;

    // Enable DMA requests from FTM
    FTM0_C2SC |= FTM_CSC_DMA | FTM_CSC_CHIE;
    //FTM0_C4SC |= FTM_CSC_DMA | FTM_CSC_CHIE;
    FTM0_C4SC |= FTM_CSC_CHIE;

    // Set initial counter
    FTM0_CNTIN = 0;

    // Enable global time base slave
    FTM0_CONF |= FTM_CONF_GTBEEN;

    // Start clocks
    FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(2);

    // Reset CNT register
    FTM0_CNT = 0;

    // Timer period
    FTM0_MOD = 47249;
    
    // Pin configuration - no slew rate
    CORE_PIN6_CONFIG |= PORT_PCR_MUX(4) | PORT_PCR_DSE;
    CORE_PIN24_CONFIG |= PORT_PCR_MUX(3) | PORT_PCR_DSE;

    // Initiate Global Time Base 
    FTM0_CONF |= FTM_CONF_GTBEOUT;
}

void pit0_setup() {
    // PIT clock gate enabled
    SIM_SCGC6 |= SIM_SCGC6_PIT;

    // PIT module enabled
    PIT_MCR = 0;

    // write 1 to clean timer interrupt flag
    PIT_TFLG0 = PIT_TFLG_TIF;

    // set timer 0 for 15 cycles
    PIT_LDVAL0 = 480000; 
}

void setup_clk() {
    ccd_clk();
    adc_clk();
    pwm_clk();
    pit0_setup();
}   

