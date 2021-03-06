/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * inbox_view.h : Code for handling all UI actions in the inbox view,
 * which is where all recv'd (and sent) messages are shown.
 */

#include <stdint.h>

/* Max number of old messages to store. */
#define NUM_MESSAGES 22

/* Initialize the inbox view. */
void inbox_init();

/* Draw the UI for viewing the inbox. */
void inbox_draw();

/* Handle any inbox specific keys. */
void inbox_handle_keypress(uint8_t key);

/* Push a message into the inbox. */
void inbox_push_message(const char *message, bit mine, const uint8_t* id);
