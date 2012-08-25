/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "compose_view.h"
#include "display.h"
#include "inbox_view.h"
#include "keys.h"
#include "message.h"
#include "puellaardens.h"
#include "radio.h"


/* Possible states the compose view can be in. */
#define COMPOSE_STATE_WRITING 0  /* writing a message */
#define COMPOSE_STATE_CONFIRM 1  /* confirm send */
#define COMPOSE_STATE_SENDING 2  /* animated progress bar */

/* File global variables. */
static __xdata char compose_buffer_[MSG_TEXT_SIZE + 22];
static __xdata uint8_t msg_id_[4];
static int8_t cursor_pos_;
static int8_t msg_len_;
static bit alt_on_;
static int8_t state_;
static uint8_t progress_;

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

  if (alt_on_) {
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
  alt_on_ = 0;
  state_ = COMPOSE_STATE_WRITING;
}

void compose_draw() {
  int8_t row, msg_len, msg_pos = 0;
  uint8_t col;
  
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
          if (alt_on_) {
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

  if (state_ == COMPOSE_STATE_WRITING) {
    if (alt_on_) {
      setCursor(7, 0);
      printf("alt keys on");
    }
  } else if (state_ == COMPOSE_STATE_CONFIRM) {
    setCursor(5, 0);
    printf("Really send? (Y/N)");
  } else if (state_ == COMPOSE_STATE_SENDING) {
    setCursor(6, 0);
    printf("Transmitting!");
    setCursor(7, 0);
    putchar('8');
    /* The division on progress_ depends on the delay and timeout */
    /* specified in message.c.  Right now timeout = 100, and with */
    /* ~25 columns, 4 should give us enough room for the whole bar. */
    for (col = 0; col < progress_ / 4; ++col) {
      putchar('=');
    }
    putchar('D');
  }

  SSN = HIGH;
}

void compose_handle_keypress(uint8_t key) {
  if (state_ == COMPOSE_STATE_WRITING) {
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
        alt_on_ = !alt_on_;
        compose_draw();
        break;
      case KSPK:
        if (alt_on_) {
          add_char('0');
          compose_draw();
        }
        break;
      case '\n':
        state_ = COMPOSE_STATE_CONFIRM;
        compose_draw();
        break;
      default:
        if (key >= 'A' && key <= 'Z') {
          add_char(key);
          compose_draw();
        }
        break;
    }
  } else if (state_ == COMPOSE_STATE_CONFIRM) {
    switch (key) {
      case 'Y':
        /* If we're still replying to a ping, hackily ignore the Y */
        /* and hope the user just tries pressing Y again. */
        if (!message_still_sending()) {
          state_ = COMPOSE_STATE_SENDING;
          message_send(compose_buffer_,
                       msg_id_);
          progress_ = 0;
          compose_draw();
        }
        break;
      case 'N':
        state_ = COMPOSE_STATE_WRITING;
        compose_draw();
        break;
    }
  }
}

bit compose_tick() {
  if (state_ == COMPOSE_STATE_SENDING) {
    if (message_still_sending()) {
      ++progress_;
      return 1;
    } else {
      if (message_send_succeeded()) {
        inbox_push_message(compose_buffer_, 1, msg_id_);
        
        /* Reset the compose view. */
        state_ = COMPOSE_STATE_WRITING;
        compose_new_message();
      
        /* Switch state to the inbox view, which also */
        /* forces an inbox redraw. */
        switch_state(STATE_VIEW);
      } else {
        /* Go back to writing, but don't clear the message. */
        state_ = COMPOSE_STATE_WRITING;
        switch_state(STATE_COMPOSE);
        display_print_message("Transmit timed out :(", 7, 0);
      }
    }
  }
  return 0;
}
