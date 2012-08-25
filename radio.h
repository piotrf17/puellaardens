/*
 * Puella Ardens - Burning Man GirlTech based IM communicator.
 *
 * radio.h : Code for talking to the hardware radio, and low level
 * sending and receiving of packets.
 */


#define RADIO_PAYLOAD_MAX 222

extern int8_t radio_last_rssi;
extern uint8_t radio_last_lqi;

extern uint8_t rf_packet_ix;  // for debugging
extern uint8_t rf_packet_n;   // for debugging
extern uint8_t __xdata rf_packet[RADIO_PAYLOAD_MAX+3];  // for debugging

/* set up radio parameters and MAC timer */
void radio_init(void);

/* Send the packet over RF */
void radio_send_packet(const uint8_t *packet, uint8_t len);

/* Go into "listen mode" */
void radio_listen();

/* Pool for a new message, if it's received shove it into packet and */
/* return the number of payload bytes received.  Else return 0. */
uint8_t radio_receive_poll (uint8_t *packet); 

/* Blocking receive, basically listen followed by poll in a loop. */
uint8_t radio_recv_packet_block(void *packet); 

/* Poll whether the radio is still transmitting */
bit radio_still_sending();


void rftxrx_ISR(void)  __interrupt (0) __using (1);
