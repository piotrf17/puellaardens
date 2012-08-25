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
#include "info_view.h"
#include "ioCCxx10_bitdef.h"
#include "keys.h"
#include "message.h"
#include "music.h"
#include "puellaardens.h"
#include "pm.h"
#include "radio.h"

/* File global variables. */
static uint8_t state_;
static bit sleepy_;

/* TODO: remove this once all testing code is gone from here. */
static __xdata uint8_t buf[RADIO_PAYLOAD_MAX];

void switch_state(int8_t new_state) {
  state_ = new_state;
  switch (state_) {
    case STATE_VIEW: inbox_draw(); break;
    case STATE_COMPOSE: compose_draw(); break;
    case STATE_INFO: info_draw(); break;
  }
}

void poll_keyboard() {
  uint8_t key = keys_get();
  if (key == 0) {
    return;
  }

  message_stop_beeps();
  
  /* Global keys. */
  if (key == KPWR) {
    sleepy_ = 1;
    return;
  }
  
  if (state_ == STATE_VIEW) {
    switch (key) {
      case KMNU:
        switch_state(STATE_COMPOSE);
        break;
      case KBYE:
        switch_state(STATE_INFO);
        break;
      default:
        inbox_handle_keypress(key);
        break;
    }
  } else if (state_ == STATE_COMPOSE) {
    switch (key) {
      case KMNU:
        switch_state(STATE_VIEW);
        break;
      case KBYE:
        switch_state(STATE_INFO);
        break;
      default:
        compose_handle_keypress(key);
        break;
    }
  } else if (state_ == STATE_INFO) {
    switch (key) {
      case KBYE:
      case KMNU:
        switch_state(STATE_VIEW);
        break;
      default:
        info_handle_keypress(key);
        break;
    }
  }
}

/* Repeater mode, for use with a standalone dev board */
void repeater_mode() {
  CLKCON = (1<<7) | (0<<6) | (0<<3) | (0<<0); //26MHz crystal oscillator for cpu and timer
  while (CLKCON & CLKCON_OSC);  //wait for clock stability
  
  P1DIR=0x03;   //LEDs on P1.1 and P1.0

#define LEDR P1_1
#define LEDG P1_0
  clock_delayms(100);

  radio_init();
  
  clock_delayms(100);

//    clear();
  while (1) {
    LEDR = 1; LEDG = 0;
  //  print_message(" ",2,0);
  //  print_message(" ",3,0);
  //  print_message("* Listening...        ",0,0);
    radio_listen();
    while (!radio_receive_poll(buf)) {
      clock_delayms(100);
      LEDG ^= 1;
  //    SSN = LOW;
  //    setCursor(0, 15*5);
  //    printf("%d %d %d", rf_packet_ix, rf_packet_n, rf_packet[0]); 
  //    SSN = HIGH;
    }
    buf[21]='\0';
 //   print_message("                                       ",1,0);
 //   print_message(buf, 1, 0);

    LEDR = 0; LEDG = 0;

    RFST = RFST_SIDLE;
    clock_delayms(3000);

    LEDR = 0; LEDG = 1;
 //   print_message("* Sending...",2,0);

    radio_send_packet(buf);
    while (radio_still_sending()) {          
      clock_delayms(100);
 //     SSN = LOW;
 //     setCursor(2, 15*5);
 //     printf("%d %d %d", rf_packet_ix, rf_packet_n, rf_packet[0]); 
 //     SSN = HIGH;
    }
 //   print_message("* SENT!", 3, 0);
    RFST = RFST_SIDLE;
    clock_delayms(100);

  }
}

/* For debugging, a simple test of the radio. */
void run_test_radio() {
  uint8_t wait_col = 55;
  uint8_t num_rcvd;

  clear();
  display_print_message("SENDING MSG", 0, 0);

  radio_send_packet("CORN MUFFIN");
  while (radio_still_sending()) {          
    clock_delayms(100);
    display_print_message(".", 0, wait_col);
    wait_col += 5;
  }

  display_print_message("SENT! WAITING..", 1, 0);
  
  num_rcvd = radio_recv_packet_block(buf);
  buf[21]='\0';
  display_print_message(buf, 2, 0);
    
  setDisplayStart(0);
  SSN = LOW;
  setCursor(3, 0);
  printf("%d bytes RSSI=%d LQI=%02X", num_rcvd, radio_last_rssi, radio_last_lqi);
  SSN = HIGH;

  RFST = RFST_SIDLE;
  clock_delayms(100);
  
  display_print_message("SENDING ANOTHER", 4, 0);

  radio_send_packet("POOP");
  while (radio_still_sending()) {          
    clock_delayms(100);
    SSN = LOW;
    setCursor(5, 0);
    printf("%d %d %d", rf_packet_ix, rf_packet_n, rf_packet[0]); 
    SSN = HIGH;
  }

  display_print_message("SENT!", 5, 0);
}

void main(void) {
  bit test_radio = 0;
  bit bounce_radio = 0;
reset:
  sleepy_ = 0;
  state_ = STATE_VIEW;
  
  if (bounce_radio) {
    repeater_mode();
  }
  
  /* Initialize system modules. */
  clock_init();
  setIOPorts();
  configureSPI();
  LCDReset();
  radio_init();

  /* Initialize app modules. */
  message_init();
  compose_init();
  inbox_init();
  info_init();

  inbox_draw();

  if (test_radio) {
    run_test_radio();
  }

  /* Main loop. */
  while (1) {
    poll_keyboard();

    /* Send and receive messages. */
    message_tick();

    /* Handle background tasks (like progress bar) */
    if (compose_tick() && state_ == STATE_COMPOSE) {
      compose_draw();
    }

    /* go to sleep (more or less a shutdown) if power button pressed */
    if (sleepy_) {
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
