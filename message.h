/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * message.h : Definitions for the structure wrapping around a received message
 * and associated limits / attributes.
 */

#include <stdint.h>

/* Max number of text lines in a message. This size corresponds
 * to about 3 lines of text on the screen.
 */
#define MSG_TEXT_SIZE 74

/* Message attributes */
#define MSG_ATTR_NEW 0x01
#define MSG_ATTR_MINE 0x02
#define MSG_ATTR_SENT 0x04

/* All the data we store about a message. */
typedef struct {
  /* Actual text of the message. */
  char text[MSG_TEXT_SIZE];

  /* Various attributes, defined above. */
  uint8_t attr;

  /* A random, hopefully unique ID on the message. */
  uint8_t id[4];
} MessageInfo;

/* very important */
#define TWENTYTWO 22

/* Initialize message module. */
void message_init();

/* Stop the beeping! */
void message_stop_beeps();

/* Tick for listening to the radio and handling incoming messages. */
void message_tick();

/* Send a message in the background. Fails silently if we're already */
/* sending a message.  Returns the 4 byte id of the sent message in id. */
void message_send(const char* buf, uint8_t* id, bit use_id);

/* Are we still sending a message? */
bit message_still_sending();

/* See if the last transmission timed out or not. */
bit message_send_succeeded();
