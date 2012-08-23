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
#include "music.h"


/* File global variables. */
static uint8_t state;
static uint8_t beeps;
static bit sleepy;
static __xdata uint8_t buf[RADIO_PAYLOAD_MAX];

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
  
  beeps=0;

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

void main(void) {
  bit test_radio = 0;
  bit bounce_radio = 0;
  uint8_t wait_col = 55;
  uint8_t num_rcvd;
reset:
  sleepy = 0;
  state = STATE_VIEW;

 
  
  if (bounce_radio)
    repeater_mode();
  
  /* Initialize system modules. */
  
  clock_init();
  setIOPorts();
  configureSPI();
  LCDReset();
  radio_init();

  /* Initialize app modules. */
  compose_init();
  inbox_init();

  inbox_draw();

  if (test_radio) {
    clear();
    print_message("SENDING MSG", 0, 0);

    radio_send_packet("CORN MUFFIN");
    while (radio_still_sending()) {          
      clock_delayms(100);
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
    printf("%d bytes RSSI=%d LQI=%02X", num_rcvd, radio_last_rssi, radio_last_lqi);
    SSN = HIGH;

    RFST = RFST_SIDLE;
    clock_delayms(100);
  
    print_message("SENDING ANOTHER", 4, 0);

    radio_send_packet("POOP");
    while (radio_still_sending()) {          
      clock_delayms(100);
      SSN = LOW;
      setCursor(5, 0);
      printf("%d %d %d", rf_packet_ix, rf_packet_n, rf_packet[0]); 
      SSN = HIGH;
    }

    print_message("SENT!", 5, 0);
  
  }

  beeps = 0;
  radio_listen();
  while (1) {
    poll_keyboard();

    if (beeps) {
      beep();
      clock_delayms(300);
      beeps--;
    }
    
    // Quick and dirty receive ability
    if (radio_receive_poll(buf)) {
      clear();
      print_message("Incoming transmission!",0,0);
      beeps = 100;
      print_message(buf,2,0);
      SSN = LOW;
      setCursor(3, 0);
      printf("RSSI: %d, LQI: %02X", radio_last_rssi, radio_last_lqi); 
      SSN = HIGH;
      inbox_push_message(buf, 0);
      radio_listen();
    }

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
