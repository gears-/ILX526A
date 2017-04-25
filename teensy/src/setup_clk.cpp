#include "Arduino.h"
#include "setup_clk.h"

void ccd_clk() {
    // Halt timers
    FTM1_SC = 0;

    // Total count number at which switch occurs
    FTM1_C0V = 24;

    // See table p783
    // This config yields edge aligned PWM with high-true pulses
    // DECAPEN = 0
    // COMBINE = 0
    // CPWMS = 0
    // MSNB, MSNA = 10 -> edge aligned PWM
    // ELSB, ELSA = 10 -> high-true pulses
    FTM1_C0SC = FTM_CSC_MSB | FTM_CSC_ELSB;    

    // Set initial count
    FTM1_CNTIN = 0;

    // Enable global time base slave
    FTM1_CONF |= FTM_CONF_GTBEEN & ~FTM_CONF_GTBEOUT;

    // Start clocks
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);

    // Reset CNT register
    FTM1_CNT = 0;

    // Timer period
    FTM1_MOD = 47;
    
    // Pin configuration - no slew rate
    CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE;

}

void adc_clk() {
    // Halt timers
    FTM1_SC = 0;

    // Count at which switch occurs
    FTM1_C1V = 24;

    // See table p783
    // This config yields edge aligned PWM with high-true pulses
    // DECAPEN = 0
    // COMBINE = 0
    // CPWMS = 0
    // MSNB, MSNA = 10 -> edge aligned PWM
    // ELSB, ELSA = 01 -> low-true pulses
    // Note that one can also do that with the COMP flag
    FTM1_C1SC = FTM_CSC_MSB | FTM_CSC_ELSA;    

    // Set initial counter
    FTM1_CNTIN = 0;

    // Enable global time base slave
    FTM1_CONF |= FTM_CONF_GTBEEN & ~FTM_CONF_GTBEOUT;

    // Start clocks
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);

    // Reset CNT register
    FTM1_CNT = 0;

    // Timer period
    FTM1_MOD = 47;
    
    // Pin configuration - no slew rate
    CORE_PIN17_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE;

    // We mask the ADC clock (channel 1) until it is triggered
    FTM1_OUTMASK = ADC_MASK; 
}

void pwm_clk() {
    // Halt timers
    FTM0_SC = 0;

    // Count that corresponds to when switch occurs
    FTM0_C4V = ROG_UP;
    FTM0_C5V = ROG_DURATION;

    FTM0_C6V = SHUT_UP;
    FTM0_C7V = SHUT_DURATION;

    // ROG, SHUT
    // See table p783
    // This config yields high true pulses on channel n match
    // DECAPEN = 0
    // COMBINE = 1
    // CPWMS = 0 
    // MSnB:MSnA = xx
    // ELSnB:ELSnA = 10
    // FTM0_CH7 works as the complement of channel 6, which has low-true pulses
    FTM0_COMBINE = FTM_COMBINE_COMBINE2 | FTM_COMBINE_COMBINE3; // Combine channels 4 and 5, and 6 and 7
    FTM0_COMBINE |= FTM_COMBINE_COMP3;
    FTM0_C4SC |= FTM_CSC_ELSB;
    FTM0_C6SC |= FTM_CSC_ELSA;

    // Set initial counter
    FTM0_CNTIN = 0;

    // Enable global time base master 
    FTM0_CONF |= FTM_CONF_GTBEEN;

    // Start clocks
    FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(2);

    // Reset CNT register
    FTM0_CNT = 0;

    // Timer period -- does not really matter, we reset the clocks separately
    FTM0_MOD = 59999;
    
    // Pin configuration - no slew rate
    CORE_PIN6_CONFIG |= PORT_PCR_MUX(4) | PORT_PCR_DSE;
    CORE_PIN5_CONFIG |= PORT_PCR_MUX(4) | PORT_PCR_DSE;

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

