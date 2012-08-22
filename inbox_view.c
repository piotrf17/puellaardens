/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "inbox_view.h"
#include "keys.h"
#include "message.h"

/* File global variables. */
static __xdata MessageInfo msg_buffer_[NUM_MESSAGES];
static uint8_t cur_msg_;
static uint8_t first_msg_;
static uint8_t last_msg_;

/* Internal functions. */

uint8_t draw_message(const MessageInfo* msg, uint8_t row) {
  uint8_t msg_len = strlen(msg->text);
  uint8_t msg_pos = 0;
  uint8_t col = 0;
  bit last_row = 0;
  
  SSN = LOW;
  setDisplayStart(0);

  /* Render the first row, which is 1 short for indicators. */
  if (msg_len <= CHAR_WIDTH - 1) {
    last_row = 1;
  }
  setCursor(row, 0);
  for (col = 0; col < CHAR_WIDTH - 1 && msg_pos < msg_len; ++col, ++msg_pos) {
    if (last_row) {
      putchar_mask(msg->text[msg_pos], 0x80);
    } else {
      putchar(msg->text[msg_pos]);
    }
  }
  if (last_row) {
    for (col; col < CHAR_WIDTH - 1; ++col) {
      putchar_mask(' ', 0x80);
    }
  }
  ++row;

  /* Render any message indicators. */
  txData(0xFF);
  txData(0x80);
  if (msg->attr & MSG_ATTR_MINE) {
    if (msg->attr & MSG_ATTR_SENT) {
      putchar_mask('M', 0x80);
    } else {
      putchar_mask('?', 0x80);
    }
  } else {
    if (msg->attr & MSG_ATTR_NEW) {
      putchar_mask('*', 0x80);
    } else {
      putchar_mask(' ', 0x80);
    }
  }

  /* Render the remaining rows of the message */
  while (row < CHAR_HEIGHT && msg_pos < msg_len) {
    if (msg_pos >= msg_len - CHAR_WIDTH) {
      last_row = 1;
    }

    /* Draw the text on this row. */
    setCursor(row, 0);
    for (col = 0; col < CHAR_WIDTH && msg_pos < msg_len; ++col, ++msg_pos) {
      if (last_row) {
        putchar_mask(msg->text[msg_pos], 0x80);
      } else {
        putchar(msg->text[msg_pos]);
      }
    }
    
    /* Finish drawing the line on the last row. */
    if (last_row) {
      for (col; col < CHAR_WIDTH; ++col) {
        putchar_mask(' ', 0x80);
      }
      txData(0x80);
      txData(0x80);
    }
    
    ++row;
  }
  
  SSN = HIGH;
  return row;
}

void move_to_next_message() {
  if (cur_msg_ != last_msg_) {
    msg_buffer_[cur_msg_].attr &= ~MSG_ATTR_NEW;
    cur_msg_ = (cur_msg_ + 1) % NUM_MESSAGES;
  }
}

void move_to_prev_message() {
  if (cur_msg_ != first_msg_) {
    cur_msg_ = (cur_msg_ + NUM_MESSAGES - 1) % NUM_MESSAGES;
  }
}

void inbox_push_message(char *message) {
  if (((last_msg_ + 1) % NUM_MESSAGES) == first_msg_) {
    // inbox is full, drop the oldest
    first_msg_++;
    first_msg_ %= NUM_MESSAGES;
  }
  last_msg_++;
  last_msg_ %= NUM_MESSAGES;
  strcpy(msg_buffer_[last_msg_].text, message);
  msg_buffer_[last_msg_].attr = MSG_ATTR_NEW;
}

void inbox_init_test_messages() {
#define ADD_TEST_MSG(_i, _msg, _attr)     \
  strcpy(msg_buffer_[_i].text, _msg); \
  msg_buffer_[_i].attr = _attr;
/*
  ADD_TEST_MSG(16, "MATT IS A DORK.  ALSO HE SMELLS FUNNY.", 0);
  ADD_TEST_MSG(17, "DINNER SERVING IN 30 MINUTES, SLOP FOR ALL.", 0);
  ADD_TEST_MSG(18, "MASSIVE WHITEOUT COMING, TAKE COVER!", MSG_ATTR_NEW | MSG_ATTR_MINE | MSG_ATTR_SENT);
  ADD_TEST_MSG(19, "THIS MESSAGE IS LONG, TO TEST OUT LONG MESSAGES.  IT IS 3 LINES LONG.", MSG_ATTR_NEW);
  ADD_TEST_MSG(20, "SWEET LEITSHOW STARTING IN 22 MINUTES", MSG_ATTR_NEW);
  ADD_TEST_MSG(21, "WILD ELMO HAS APPEARED.", MSG_ATTR_NEW | MSG_ATTR_MINE);
  ADD_TEST_MSG(0, "SNELLA CAME BY THE DOME AND ATE ALL THE FOOD", MSG_ATTR_NEW);
  ADD_TEST_MSG(1, "ANYONE WANT TO TIME TRAVEL?  LEAVING 5 MINUTES AGO", MSG_ATTR_NEW);
  ADD_TEST_MSG(2, "TOILETS ARE ALL FULL #POOPTROUBLES", MSG_ATTR_NEW);
  ADD_TEST_MSG(3, "HENRY HAS A STINKY BUTT", MSG_ATTR_NEW);
*/

  ADD_TEST_MSG(0, "Inbox: Test message", MSG_ATTR_NEW);
#undef ADD_TEST_MSG
  /*
  cur_msg_ = 18;
  first_msg_ = 16;
  last_msg_ = 3;
  */
}

/* Public API. */

void inbox_init() {
  cur_msg_ = 0;
  first_msg_ = 0;
  last_msg_ = 0;

  /* For testing purposes. */
  /* TODO: remove this function. */
  inbox_init_test_messages();
}

void inbox_draw() {
  uint8_t row, msg;

  clear();

  row = 0;
  for (msg = cur_msg_;
       msg != (last_msg_ + 1) % NUM_MESSAGES && row < CHAR_HEIGHT;
       msg = (msg + 1) % NUM_MESSAGES) {
    row = draw_message(&msg_buffer_[msg], row);
  }
}

void inbox_handle_keypress(uint8_t key) {
  switch (key) {
    case '^':
    case '<':
      move_to_prev_message();
      inbox_draw();
      break;

    case KDWN:
    case '>':
      move_to_next_message();
      inbox_draw();
      break;

  }
}
