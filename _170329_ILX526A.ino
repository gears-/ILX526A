#include <util/delay_basic.h>

#define ROG_PIN 0x04  // b00000100 - PD2
#define CCD_CLK 0x08  // b00001000 - PD3
#define SHUT_PIN 0x10 // b00010000 - PD4
#define ADC_CLK 0x40  // b01000000 - PD6
#define DBG_PIN 0x80 // b10000000 - PD7

void setup()
{

  DDRD = DDRD | B11111100;
  PORTD = 0x00;
  /*
   * Generate a 4 MHz signal on pin #6 w/ CTC mode for ADC
   */
  cli();
  
  // Clean up registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 1;
  TIMSK0 = 0;

  OCR0A = 1;
  OCR0B = 1; 

  TCCR0A = bit(COM0A0) | bit(COM0B0); // Enable outputs on both pins
  TCCR0A |= bit(WGM01); // CTC Mode
  TCCR0B = bit(CS00); // No prescaler
//
//  TCCR1A = 0;
//  TCCR1B = 0;
//  TCNT1 = 0;
//  TIMSK1 = 0;
//
//  TCCR1A = bit(COM1A0);
//  TCCR1B = bit(CS10);
//  TCCR1B |= bit(WGM12);
//
//  OCR1A = 7;
//  OCR1B = 0;

//  /*** Set up the CCD clock
//   *  
//   */
   TCCR2A = 0;
   TCCR2B = 0;
   TIMSK2 = 0;
   TCCR2A = bit(COM2B0) | bit(WGM21);
   TCCR2B = bit(CS20);
   OCR2A = 7;
   OCR2B = 7;
 

  sei();
}


void loop() {
  // Disable interrupts
  cli();

  uint8_t tmp = 0;

  tmp = PORTD;
  // Set ROG high and SHUT low
  tmp |= ROG_PIN;
  tmp &= ~SHUT_PIN;

  //// Synchronize clocks
  // Wait for CCD_CLK to be high then low
  while(!(PIND & CCD_CLK));
  while((PIND & CCD_CLK));
 
  
  // Reset counters - Note that the ADC clock is going to be behind by a few master clock cycles
  TCNT0 = 0;
  TCNT2 = 0;

  //// Set PORTD
  // Since we start on the "low" of the clock, wait for the next cycle. That's 500 ns = 8 cycles at 16 MHz
  _delay_loop_2(2); // 2*4 = 8 cycles 500 ns
  // PORTD = tmp gets executed within one 16 MHz cycle => lags behind clock by 62.5 ns
  // Need to add a few 62.5 ns delays to comply with timing requirements
  _delay_loop_1(1); // 1*3 = 3 cycles
  PORTD = tmp;
  
  // Wait for 160 cycles (160 @ 16 MHz = 10 @ 1 MHz), but shave a few cycles to account for delay in execution of PORTD
  // Make sure we fall within the timing requirements
  _delay_loop_2(39); // 39*4 = 156 CPU cycles 4 uS
  __asm__("nop");

  //// Set ROG low 
  PORTD ^= ROG_PIN;

  //// Sync back with the clock; we're off by 12 cycles at this point
  _delay_loop_2(3);

  //// Wait for 57 dummy pixels to clear out - 57 @ 1 MHz = 912 @ 16 MHz
  _delay_loop_2(228); // 228*4 = 912 CPU cycles 57 uS


  //// Start reading pixels - 3000 of them
  _delay_loop_2(12000); // 12000*4 = 48k CPU cycles 
    
  //// Wait for 2 dummy pixels to clear out
  _delay_loop_2(8); 

  //// Wait for 1/2 of next cycle @ 1 MHz, then add some padding to comoply with timing requirements before setting shutter pin 
  _delay_loop_2(2);
  __asm__("nop");
  
   // Set SHUT high for 5 @1 MHz cycles, but shave few cycles to account for delay in execution of PORTD
  PORTD |= SHUT_PIN;
  _delay_loop_2(19);
  __asm__("nop"); 
  
  // Set SHUT low to indicate end of cycle
  PORTD ^= SHUT_PIN;
  
  //// Wait before we can loop again
  // 32 1 MHz cycles = 512 16 MHz cycles
  _delay_loop_2(128); // 512 = 128*4
//
  sei();
  
}
