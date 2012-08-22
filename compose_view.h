/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * compose_view.h : Code for handling all UI actions in the message
 * composition view.
 */

#include <stdint.h>

/* Initialize the compose view. */
void compose_init();

/* Draw the UI for composing a message. */
void compose_draw();

/* Handle any compose specific keys. */
void compose_handle_keypress(uint8_t key);
