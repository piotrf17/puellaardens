/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>

#include "clock.h"
#include "display.h"
#include "keys.h"
#include "radio.h"

#define MAX_PINGS 12

static int8_t __xdata pings_[MAX_PINGS];
static int8_t num_pings_;

/* Ping for nearby girltechs. */
void ping() {
  num_pings_ = 0;
  info_draw();
  
  display_print_message("Pinging now...", 2, 0);
  radio_send_packet("p");
  while (radio_still_sending()) {
    clock_delayms(400);
  }
  radio_listen();
  
  display_print_message("pung", 2, 70);
}

/* Public API */

/* Someone pinged us. */
void info_gotping() {
  radio_send_packet("o");
  while (radio_still_sending()) {
    clock_delayms(50);
  }
  radio_listen();
}

/* We got back a ping. */
void info_gotpong() {
  if (num_pings_ < MAX_PINGS) {
    pings_[num_pings_] = radio_last_rssi;
    num_pings_++;
  }
}

void info_init() {
  num_pings_ = 0;
}

void info_draw() {
  int8_t i, row, col;
  
  clear();

  SSN = LOW;
  setDisplayStart(0);
  setCursor(0, 0);
  printf("Radio Status:");

  setCursor(1, 0);
  printf("RSSI: %d, LQI: %02X", radio_last_rssi, radio_last_lqi);

  if (num_pings_) {
    setCursor(2, 0);
    printf("Results from last ping:");
  }
  i = 0; row = 3; col = 0;
  while (i < num_pings_) {
    setCursor(row, col);
    printf("%d", pings_[i]);
    col += 40;
    if (col > 80) {
      col = 0;
      ++row;
    }
    ++i;
  }
  
  SSN = HIGH;
}

void info_handle_keypress(uint8_t key) {
  switch (key) {
    case KONL:
      ping();
      break;
  }
}
