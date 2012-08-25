/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>

#include "clock.h"
#include "display.h"
#include "inbox_view.h"
#include "info_view.h"
#include "keys.h"
#include "message.h"
#include "music.h"
#include "radio.h"

/* Possible states for the message module. */
#define MESSAGE_STATE_LISTEN  0
#define MESSAGE_STATE_SENDING 1

/* File global variables */
static __xdata uint8_t buf_[RADIO_PAYLOAD_MAX];
static int8_t beeps_;
static int8_t state_;
static uint8_t timeout_;
static bit last_failed_;

/* Internal functions. */

void handle_command() {
  switch(buf_[0]) {
    case 'p':
      info_gotping();
      break;
    case 'o':
      info_gotpong();
      break;
  }
}

/* Public API */

void message_init() {
  beeps_ = 0;
  state_ = MESSAGE_STATE_LISTEN;
  last_failed_ = 0;
  radio_listen();
}

void message_stop_beeps() {
  beeps_ = 0;
}

void message_send(const char* buf) {
  state_ = MESSAGE_STATE_SENDING;
  radio_send_packet(buf);

  /* 85 byte max message @ 50 baud = 13 seconds. */
  /* With 100ms delay, set timeout to 150. */
  timeout_ = 150;
}

bit message_still_sending() {
  return state_ == MESSAGE_STATE_SENDING;
}

bit message_send_succeeded() {
  return !last_failed_;
}

void message_tick() {
  if (state_ == MESSAGE_STATE_SENDING) {
    if (radio_still_sending()) {
      clock_delayms(100);

      if (--timeout_ == 0) {
        state_ = MESSAGE_STATE_LISTEN;
        last_failed_ = 1;
        radio_listen();
      }
    } else {
      state_ = MESSAGE_STATE_LISTEN;
      last_failed_ = 0;
      radio_listen();
    }
  } else {
  
    /* Beep annoyingly if we've received a message. */
    /* This should last 5.5 seconds. */
    if (beeps_) {
      beep();
      clock_delayms(250);
      --beeps_;
    }

    if (radio_receive_poll(buf_)) {
      clock_delayms(50);  /* voodoo delay */
      radio_listen();

      /* Hack: special command messages are sent in lower case */
      /* since normal messages all appear in upper case. */
      if (buf_[0] >= 'a' && buf_[0] <= 'z') {
        handle_command();
      } else {
        beeps_ = TWENTYTWO;
        inbox_push_message(buf_, 0);
        inbox_draw();
      }
    }
  }
}
