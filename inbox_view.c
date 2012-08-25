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

#define ADD_TEST_MSG(_i, _msg, _attr) \
  strcpy(msg_buffer_[_i].text, _msg); \
  msg_buffer_[_i].attr = _attr;

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
  char start_char = '>';
  
  SSN = LOW;
  setDisplayStart(0);

  /* Render the first row, which is 2 short for indicators. */
  if (msg_len <= CHAR_WIDTH - 2) {
    last_row = 1;
  }
  setCursor(row, 0);
  if (msg->attr & MSG_ATTR_MINE) {
    start_char = '$';
  }
  if (last_row) {
    putchar_mask(start_char, 0x80);
  } else {
    putchar(start_char);
  }
  for (col = 1; col < CHAR_WIDTH - 1 && msg_pos < msg_len; ++col, ++msg_pos) {
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

/* Public API. */

void inbox_init() {
  cur_msg_ = 0;
  first_msg_ = 0;
  last_msg_ = 0;

  /* Provide some intro instruction messages. */
  ADD_TEST_MSG(0, "Welcome to grlltech.", 0);
  ADD_TEST_MSG(1, "Scroll messages with the wheel on the left.", 0);
  ADD_TEST_MSG(2, "Hit menu to switch to compose view, and menu again to return.", 0);
  ADD_TEST_MSG(3, "Press Enter while composing to send your message.", 0);
  last_msg_ = 3;
}

void inbox_draw() {
  uint8_t row, msg;

  clear();

  row = 0;
  /* To handle a boundary condition, render the first message seperately. */
  if (first_msg_ != last_msg_) {
    row = draw_message(&msg_buffer_[cur_msg_], row);
  }
  for (msg = cur_msg_ + 1;
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

void inbox_push_message(char *message, bit mine) {
  /* If inbox is full, drop the oldest message. */
  if (((last_msg_ + 1) % NUM_MESSAGES) == first_msg_) {
    first_msg_ = (first_msg_ + 1) % NUM_MESSAGES;
    first_msg_ %= NUM_MESSAGES;
  }
  
  last_msg_ = (last_msg_ + 1) % NUM_MESSAGES;
  strcpy(msg_buffer_[last_msg_].text, message);
  msg_buffer_[last_msg_].attr = MSG_ATTR_NEW;
  if (mine) {
    msg_buffer_[last_msg_].attr |= MSG_ATTR_MINE;
  }
}
