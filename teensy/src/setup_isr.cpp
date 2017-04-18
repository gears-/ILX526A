/* Function: setup_shut_isr
 *  Description: setups the interrupt service routine raised by SHUT falling down
 *  THe ISR takes care of
 *  1. Stopping the ADC
 *  2. Check if we reached the requested number of samples, and send if data if we did
 *  3. Accumulate data in the send buffer if we did not reach the overall number of requested samples
 */

#include "setup_dma.h"
#include "setup_isr.h"

#define NSAMPLES 1
#define DUMMY_DELTA (6*57)
 
void setup_shut_isr() {
    // Enable ISR on falling edge of pin 6 (SHUT)
    CORE_PIN6_CONFIG |= PORT_PCR_IRQC(10);

    // Enable PORTD interrupts in the NVIC table
    NVIC_ENABLE_IRQ(IRQ_PORTD);
  
}extern volatile uint16_t pix_buffer[6*(NPIX+100)];

void portd_isr() {
  // Clear interrupts on PORTD
  PORTD_PCR4 &= ~PORT_PCR_ISF;

  // Disable the ADC
  FTM2_OUTMASK |= 0xFF;

  // Increment the samples counter
  nsamples_gathered++;

  // DEBUG -- prevent the ADC from starting again
  dma_adc_start.disable();
  
  // Have we reached the threshold to send data?
  if(nsamples_gathered == NSAMPLES) {
    // Make sure we average correctly
    for(int i=0;i<NPIX;++i) {
      buffer_send[i] /= (6*NSAMPLES);
    }  
    // Reset nsamples_gathered
    nsamples_gathered = 0;     

    // Send our buffer away...
  } 
  // Otherwise, accumulate data from the pixel buffer
  else {
    for(int i=0;i<NPIX;++i) {
      for(int j=0;j<6;++j) {
        buffer_send[i] += pix_buffer[DUMMY_DELTA + 6*i + j];
      }
    }
  } 
}

