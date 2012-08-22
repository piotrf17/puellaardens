/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * puellaardens.c : Main entry point.
 */


#include <cc1110.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "5x7.h"
#include "clock.h"
#include "compose_view.h"
#include "display.h"
#include "inbox_view.h"
#include "ioCCxx10_bitdef.h"
#include "keys.h"
#include "message.h"
#include "puellaardens.h"
#include "pm.h"
#include "radio.h"

/* File global variables. */
static uint8_t state;
static bit sleepy;

void switch_state(int8_t new_state) {
  state = new_state;
  switch (state) {
    case STATE_VIEW: inbox_draw(); break;
    case STATE_COMPOSE: compose_draw(); break;
  }
}

void poll_keyboard() {
  uint8_t key = keys_get();
  if (key == 0) {
    return;
  }

  if (state == STATE_VIEW) {
    switch (key) {
      case KMNU:
        state = STATE_COMPOSE;
        compose_draw();
        break;
      case KPWR:
        sleepy = 1;
        break;
      default:
        inbox_handle_keypress(key);
        break;
    }
  } else if (state == STATE_COMPOSE) {
    switch (key) {
      case KMNU:
        state = STATE_VIEW;
        inbox_draw();
        break;
      case KPWR:
        sleepy = 1;
        break;
      default:
        compose_handle_keypress(key);
        break;
    }
  }
}

/* Convenience function for debugging. */
void print_message(const char* msg, int row, int col) {
  setDisplayStart(0);
  SSN = LOW;
  setCursor(row, col);
  printf(msg);
  SSN = HIGH;
}

void main(void) {
  char buf[22];
  bit test_radio = 0;
  uint8_t wait_col = 55;
  uint8_t num_rcvd;
  
reset:
  sleepy = 0;
  state = STATE_VIEW;

  /* Initialize system modules. */
  clock_init();
  setIOPorts();
  configureSPI();
  LCDReset();

  /* Initialize app modules. */
  compose_init();
  inbox_init();
  
  inbox_draw();

  if (test_radio) {
    /* Setup radio. */
    EA = 1;       // Enable interrupts.
    radio_init();

    clear();
    print_message("SENDING MSG", 0, 0);

    radio_send_packet("CORN MUFFIN");
    while (radio_still_sending()) {          
      clock_delayms(1000);
      print_message(".", 0, wait_col);
      wait_col += 5;
    }

    print_message("SENT! WAITING..", 1, 0);
  
    num_rcvd = radio_recv_packet_block(buf);
    buf[21]='\0';
    print_message(buf, 2, 0);
    
    setDisplayStart(0);
    SSN = LOW;
    setCursor(3, 0);
    printf("%d bytes recvd", num_rcvd);
    SSN = HIGH;
  }

  while (1) {
    poll_keyboard();

    /* go to sleep (more or less a shutdown) if power button pressed */
    if (sleepy) {
      clear();
      clock_delayms(1000);
      SSN = LOW;
      LCDPowerSave();
      SSN = HIGH;
      sleep();
      /* reset on wake */
      goto reset;
    }
  }
}
