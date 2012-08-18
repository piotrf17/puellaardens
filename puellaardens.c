/*
 * Puella Ardens
 *
 * Burning Man GirlTech based IM communicator.
 */


#include <cc1110.h>
#include <stdint.h>
#include "ioCCxx10_bitdef.h"
#include "display.h"
#include "keys.h"
#include "5x7.h"
#include "stdio.h"
#include "puellaardens.h"
#include "pm.h"

/* globals */
bit sleepy;

void draw() {
  uint8_t col;
  
  SSN = LOW;
  setDisplayStart(0);

  setNormalReverse(0);

  setCursor(0, 0);
  for (col = 0; col < WIDTH; col++) {
    txData(0xFF);
  }
    
  SSN = HIGH;
}

void print_message(const char* msg, int row, int col) {
  setDisplayStart(0);
  SSN = LOW;
  setCursor(row, col);
  printf(msg);
  SSN = HIGH;
}

void poll_keyboard() {
  switch (getkey()) {
    case 'H':
      print_message("henry is a dork", 1, 0);
      break;
    case 'P':
      print_message("piotr is cooler", 1, 0);
      break;
    case 'M':
      print_message("matt is a dork ", 1, 0);
      break;
    case KPWR:
      sleepy = 1;
      break;
    default:
      break;
  }
}

void main(void) {
  
reset:
  sleepy = 0;

  xtalClock();
  setIOPorts();
  configureSPI();
  LCDReset();

  draw();
  print_message("matt is a dork", 1, 0);

  while (1) {
    poll_keyboard();

    /* go to sleep (more or less a shutdown) if power button pressed */
    if (sleepy) {
      clear();
      sleepMillis(1000);
      SSN = LOW;
      LCDPowerSave();
      SSN = HIGH;
      sleep();
      /* reset on wake */
      goto reset;
    }
  }
}
