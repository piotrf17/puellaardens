/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>

#include "clock.h"
#include "display.h"
#include "info_view.h"
#include "keys.h"
#include "message.h"
#include "music.h"
#include "radio.h"

#define MAX_PINGS 12

/* File global variables. */
static int8_t __xdata pings_[MAX_PINGS];
static uint8_t __xdata ping_id_[4];
static uint8_t __xdata pong_id_[4];
static int8_t num_pings_;
bit need_redraw_;

/* Ping for nearby girltechs.  The ping is sent in the foreground. */
void ping() {
  num_pings_ = 0;
  info_draw();
  
  display_print_message("Pinging now...", 2, 0);
  message_send("p", ping_id_);
  while (message_still_sending()) {
    message_tick();
  }
  
  display_print_message("pung", 2, 70);
}

/* Public API */

/* Someone pinged us. */
void info_gotping() {
  beep();

  /* If we're currently sending a message, ignore the ping. */
  if (!message_still_sending()) {
    message_send("o", pong_id_);
  }
}

/* We got back a ping. */
void info_gotpong() {
  if (num_pings_ < MAX_PINGS) {
    pings_[num_pings_] = radio_last_rssi;
    num_pings_++;
    need_redraw_ = 1;
  }
}

void info_init() {
  num_pings_ = 0;
  need_redraw_ = 0;
}

void info_draw() {
  int8_t i, row, col;

  need_redraw_ = 0;
  
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
      /* Only allow ping if we're not already sending a message. */
      if (!message_still_sending()) {
        ping();
      }
      break;
  }
}

bit info_tick() {
  return need_redraw_;
}
