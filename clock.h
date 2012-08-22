/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * clock.h : Routines for timing.
 */

/* Set system clock source to 26 Mhz. */
void clock_init();

/* Approximate delay loop. */
void clock_delayms(int ms);
