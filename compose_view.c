/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 */

#include <cc1110.h>
#include <stdio.h>
#include <string.h>

#include "compose_view.h"
#include "display.h"
#include "message.h"

/* File global variables. */
static __xdata char compose_buffer[MSG_TEXT_SIZE];
static uint8_t cursor_pos;

/* Internal functions. */

static void add_char(char c) {
  compose_buffer[cursor_pos] = c;
  compose_buffer[cursor_pos + 1] = '\0';
  ++cursor_pos;
}

/* Public API. */

void compose_new_message() {
  cursor_pos = 0;
  compose_buffer[0] = '\0';  
}

void compose_draw() {
  uint8_t row, msg_len, msg_pos = 0, col;
  
  clear();

  SSN = LOW;
  setDisplayStart(0);
  setCursor(0, 0);
  printf("Enter your message below:");

  row = 1;
  msg_len = strlen(compose_buffer);
  while (row < CHAR_HEIGHT && msg_pos < msg_len) {
    setCursor(row, 0);
    for (col = 0; col < CHAR_WIDTH && msg_pos < msg_len; ++col, ++msg_pos) {
      putchar(compose_buffer[msg_pos]);
    }
    row += 1;
  }

  SSN = HIGH;
}

void compose_handle_keypress(uint8_t key) {
  if (key >= 'A' && key <= 'Z') {
    add_char(key);
    compose_draw();
  }
}
