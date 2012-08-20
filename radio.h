#define PAYLOAD_BYTES 22


void radio_init(void);  //set up radio parameters and MAC timer
void radio_send_packet(const void *packet); //send the packet over RF
uint8_t radio_recv_packet_block(void *packet);  //go into receive mode, wait for a complete packet, write it to *packet
char radio_still_sending(); //Poll whether the radio is still transmitting

void rftxrx_ISR(void)  __interrupt (0) __using (1);
