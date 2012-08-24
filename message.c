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

/* File global variables */
static __xdata uint8_t buf_[RADIO_PAYLOAD_MAX];
static int8_t beeps_;

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
}

void message_stop_beeps() {
  beeps_ = 0;
}

void message_tick() {
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
