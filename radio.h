#define RADIO_PAYLOAD_MAX 222

extern int8_t radio_last_rssi;
extern uint8_t radio_last_lqi;

extern uint8_t rf_packet_ix;  // for debugging
extern uint8_t rf_packet_n;   // for debugging
extern uint8_t __xdata rf_packet[RADIO_PAYLOAD_MAX+3];  // for debugging

void radio_init(void);  //set up radio parameters and MAC timer

void radio_send_packet(const void *packet); //send the packet over RF

void radio_listen();                          // Go into "listen mode"
uint8_t radio_receive_poll (uint8_t *packet); // Did we receive a complete message?
                                              // If so, shoves it in packet and return number of payload bytes received.
                                              // Otherwise returns 0.
                                              
uint8_t radio_recv_packet_block(void *packet);  // Blocking receive using the above two functions 

/* Poll whether the radio is still transmitting, if so returns
 * approximately the number of packets left to recieve. */
uint8_t radio_still_sending();


void rftxrx_ISR(void)  __interrupt (0) __using (1);
