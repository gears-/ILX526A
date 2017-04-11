#include <util/delay_basic.h>

#define ROG_PIN 0x04  // b00000100 - PD2
#define CCD_CLK 0x08  // b00001000 - PD3
#define SHUT_PIN 0x10 // b00010000 - PD4
#define ADC_CLK 0x40  // b01000000 - PD6
#define DBG_PIN 0x80 // b10000000 - PD7

void setup()
{
  
  //// We will be working with PORTD
  DDRD = DDRD | B11111100;
  PORTD = 0x00;

  cli();
   //// Generate a 4 MHz signal on pin #6 w/ CTC mode for ADC 
  // Clean up registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;
  TIMSK0 = 0;

  OCR0A = 1;
  OCR0B = 1; 

  TCCR0A = bit(COM0A0) | bit(COM0B0); // Enable outputs on both pins
  TCCR0A |= bit(WGM01); // CTC Mode
  TCCR0B = bit(CS00); // No prescaler

  //// Set up the CCD clock
  // Clean up registers
   TCCR2A = 0;
   TCCR2B = 0;
   TIMSK2 = 0;
   TCNT2 = 0;

   // CTC Mode, enable output on OCR2B
   TCCR2A = bit(COM2B0) | bit(WGM21);
   TCCR2B = bit(CS20); // No prescaler
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
  // Wait for both ADC and CCD clocks to be high then low
  while(!(PIND & CCD_CLK) && (PIND & ADC_CLK));
  while((PIND & CCD_CLK) && !(PIND & ADC_CLK));
 
  // Reset counters
  TCNT0 = 0;
  TCNT2 = 0;
    
  //// Set PORTD
  // CCD CLK
  //     <- 750 ns ->          <-500 ns->
  //    without delay             with delay
  //     ___V________          ____V____
  // ___|            |________|         |___
  //    <--->                  <-->
  //    312.5 ns               250 ns
  // We are now starting on the top of the 1 MHz clock (see above); let's wait for the next 1 MHz clock tick.
  // This means we have to wait 16 cycles @ 16 MHz (1 uS)
  _delay_loop_2(4); // 4*4 = 16 cycles 1 uS
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

  //// Sync back with the clock; we're off by 12 cycles at this point before the next up on CCD clock
  _delay_loop_2(3);

  //// Wait for 57 dummy pixels to clear out - 57 @ 1 MHz = 912 @ 16 MHz
  // Dummy pixels: -1, 0, D1-D55
  _delay_loop_2(228); // 228*4 = 912 CPU cycles 57 uS

  //// Start reading pixels - 3000 of them (S1-S3000)
  _delay_loop_2(12000); // 12000*4 = 48k CPU cycles 
    
  //// Wait for 2 dummy pixels to clear out (D56, D57)
  _delay_loop_2(8); 

  //// Wait for 1/2 of next cycle @ 1 MHz, then add some padding to comply with timing requirements before setting shutter pin 
  _delay_loop_2(2);
  __asm__("nop");
  
   // Set SHUT high for 5 @1 MHz cycles, but shave few cycles to account for delay in execution of PORTD
  PORTD |= SHUT_PIN;
  _delay_loop_2(19);
  __asm__("nop"); 
  
  // Set SHUT low to indicate end of cycle
  PORTD ^= SHUT_PIN;
  
  //// Wait before we can loop again
  // 31.5 1 MHz cycles = 504 16 MHz cycles
  // The 0.5 ensures we end up at the same spot every time
  _delay_loop_2(126); // 504 = 126*4

  sei();
//  
}
