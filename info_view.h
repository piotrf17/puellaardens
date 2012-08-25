/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * info_view.h : Code for handling the s3kr1t radio info view, where we can
 * see last transmission LQI, RSSI, as well as ping nearby communicators.
 */

/* Initialize the info view. */
void info_init();

/* Draw the UI for the info view. */
void info_draw();

/* Handle any info view specific keys. */
void info_handle_keypress(uint8_t key);

/* ping/pongs */
void info_gotping(const uint8_t* ping_id);
void info_gotpong(const uint8_t* ping_id);

/* Handle background tasks, returns 1 if needs redraw. */
bit info_tick();
