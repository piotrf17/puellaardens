/*
 * Puella Ardens
 *
 * Burning Man GirlTech based IM communicator.
 */

/* Max number of old messages to store. */
#define NUM_MESSAGES 22

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
} MessageInfo;
