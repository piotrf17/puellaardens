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

/* Handle any compose specific keys. This function assumes */
/* we are in compose view and may redraw the screen. */
void compose_handle_keypress(uint8_t key);

/* Handle possibly background tasks, returns 1 if it wants the*/
/* view redrawn, but doesn't actually redraw anything. */
bit compose_tick();
