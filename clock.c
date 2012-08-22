/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * Copyright 2010 Dave
 * http://daveshacks.blogspot.com/2010/01/im-me-lcd-interface-hacked.html
 *
 * Copyright 2010 Michael Ossmann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <cc1110.h>

#include "clock.h"
#include "ioCCxx10_bitdef.h"

void clock_init() {
  /* Turn both high speed oscillators on. */
  SLEEP &= ~SLEEP_OSC_PD;

  /* Wait until xtal oscillator is stable. */
  while( !(SLEEP & SLEEP_XOSC_S) );

  /* Select xtal osc, 26 MHz */
  CLKCON = (CLKCON & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKSPD_DIV_1;
  while (CLKCON & CLKCON_OSC);

  /* Turn off the RC oscillator */
  SLEEP |= SLEEP_OSC_PD;
}

void clock_delayms(int ms) {
  int j;
  while (--ms > 0) { 
    for (j=0; j<1200;j++); // about 1 millisecond
  };
}
