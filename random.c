/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdint.h>

#include "ioCCxx10_bitdef.h"

void random_init() {
  /* Seed the RNG. */
  uint16_t seed = 0x0;
  volatile static uint8_t dummy;

  ADCCON3 = 0x0E;         /* Sample temperature */
  while(!(ADCCON1 & 0x80));
  seed |= ((uint16_t) ADCL) << 8;
  dummy = ADCH;
  
  ADCCON3 = 0x0E;         /* Sample temperature */
  while(!(ADCCON1 & 0x80));
  seed |= ADCL;
  dummy = ADCH;
    
  /* Need to write to RNDL twice to seed it */
  RNDL = seed & 0xFF;
  RNDL = seed >> 8;
}

uint8_t random_byte() {
  /* Clock the LFSR once for a random value. */
  ADCCON1 |= ADCCON1_RCTRL0;

  return RNDL;
}
