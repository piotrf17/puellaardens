/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>
#include <string.h>

#include "compose_view.h"
#include "display.h"
#include "keys.h"
#include "message.h"

/* File global variables. */
static __xdata char compose_buffer_[MSG_TEXT_SIZE];
static int8_t cursor_pos_;
static int8_t msg_len_;
static bit alt_on;

/* Internal functions. */

static void cursor_left() {
  if (cursor_pos_ > 0) {
    --cursor_pos_;
  }
}

static void cursor_right() {
  if (cursor_pos_ < msg_len_) {
    ++cursor_pos_;
  }
}

static void add_char(char c) {
  int8_t i;

  if (alt_on) {
    c = keys_altkey(c);
  }
  
  if (msg_len_ < MSG_TEXT_SIZE - 1) {
    for (i = msg_len_ + 1; i >= cursor_pos_; --i) {
      compose_buffer_[i] = compose_buffer_[i - 1];
    }
    compose_buffer_[cursor_pos_] = c;
    ++cursor_pos_;
    ++msg_len_;
  }
}

static void del_char() {
  int8_t i;
  
  if (cursor_pos_ > 0) {
    for (i = cursor_pos_; i < msg_len_ + 1; ++i) {
      compose_buffer_[i - 1] = compose_buffer_[i];
    }
    --cursor_pos_;
    --msg_len_;
  }
}

static void compose_new_message() {
  cursor_pos_ = 0;
  msg_len_ = 0;
  compose_buffer_[0] = '\0';
}

/* Public API. */

void compose_init() {
  compose_new_message();
  alt_on = 0;
}

void compose_draw() {
  int8_t row, msg_len, msg_pos = 0, col;
  
  clear();

  SSN = LOW;
  setDisplayStart(0);
  setCursor(0, 0);
  printf("Enter your message below:");

  row = 1;
  msg_len = strlen(compose_buffer_);
  while (row < CHAR_HEIGHT && msg_pos < msg_len + 1) {
    setCursor(row, 0);
    for (col = 0; col < CHAR_WIDTH && msg_pos < msg_len + 1; ++col, ++msg_pos) {
      if (msg_pos == msg_len) {
        if (msg_pos == cursor_pos_) {
          if (alt_on) {
            putchar_mask('^', 0x80);
          } else {
            putchar_mask(' ', 0x80);
          }
        }
      } else {
        if (msg_pos == cursor_pos_) {
          putchar_mask(compose_buffer_[msg_pos], 0x80);
        } else {
          putchar(compose_buffer_[msg_pos]);
        }
      }
    }
    row += 1;
  }

  if (alt_on) {
    setCursor(7, 0);
    printf("alt keys on");
  }

  SSN = HIGH;
}

void compose_handle_keypress(uint8_t key) {
  switch (key) {
    case KBACK:
      del_char();
      compose_draw();
      break;
    case '<':
      cursor_left();
      compose_draw();
      break;
    case '>':
      cursor_right();
      compose_draw();
      break;
    case ' ':
    case ',':
      add_char(key);
      compose_draw();
      break;
    case KALT:
      alt_on = !alt_on;
      compose_draw();
      break;
    case KSPK:
      if (alt_on) {
        add_char('0');
        compose_draw();
      }
      break;
    default:
      if (key >= 'A' && key <= 'Z') {
        add_char(key);
        compose_draw();
      }
      break;
  }
}
