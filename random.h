/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * random.h : Interface to on chip pseudo-random number generator
 */

#include <stdint.h>

/* Initialize hardware RNG. */
void random_init();

/* Get one random byte from the RNG. */
uint8_t random_byte();
