#include "cc1111.h"
#include <string.h>
#include "radio.h"

static uint8_t __xdata rf_packet[RADIO_PAYLOAD_MAX+3];
static uint8_t rf_packet_ix = 0;
static uint8_t rf_packet_n = 0;

int8_t radio_last_rssi;
uint8_t radio_last_lqi;

static char rf_mode_tx = 0; // controls whether the rftxrx ISR is transmitting or receiving

void rftxrx_ISR(void)  __interrupt (0) __using (1) {
  
  if (rf_mode_tx)
    RFD = rf_packet[rf_packet_ix++];  // (TX) send data to radio peripheral
  else {
    rf_packet[rf_packet_ix++] = RFD;  // (RX) write received byte to buffer
    if (rf_packet_ix == 1)      // Variable length packets.  The first byte received specifies the payload length.
      rf_packet_n = rf_packet[0] + 3; // 1st byte = payload length.  Then the payload, then RSSI and CRC_LQI.
  }

  if (rf_packet_ix == rf_packet_n) 
    RFTXRXIE = 0;    //Packet is complete, so disable the interrupt.
}

// 12 corn muffin 0 rssi lqi 
// 0             12  13  14
//
char radio_still_sending() { // Poll whether the radio is still transmitting
//  return RFTXRXIE;
  //    return (rf_packet_ix < rf_packet_n);
  return !(RFIF & RFIF_IM_DONE);
}

void radio_listen() { // Go into "listen mode"
//    RFST = RFST_SIDLE;
  
    rf_packet_n   = -1;    // rf_packet_n will be updated in the interrupt after the first byte is received (specifying the payload length)
    rf_packet_ix  = 0;
    rf_mode_tx    = 0;
    
    RFST = RFST_SRX;  
    RFTXRXIE = 1;
    
    RFIF &= ~RFIF_IM_DONE;
}

uint8_t radio_receive_poll (uint8_t *packet) { // Did we receive a complete message?
  if (RFTXRXIE)
    return 0;   // Nope (either we haven't even started receiving, or it's partially complete)
  else {
    memcpy(packet, &rf_packet[1], rf_packet_n - 3); // don't include the first byte (length byte)
    radio_last_rssi = rf_packet[rf_packet_n - 2];   // RSSI and LQI are appended to the packet
    radio_last_lqi  = rf_packet[rf_packet_n - 1];
    return rf_packet_n - 3;
  }
}

uint8_t radio_recv_packet_block(void *packet) {  //go into receive mode, wait for a complete packet, write it to *packet
  
    uint8_t n;

    radio_listen();

    while (!(n = radio_receive_poll(packet)));
  
    return n;
}

void radio_send_packet(const void *packet) { //send the packet over RF
      rf_packet[0] = strlen(packet) + 1;  // payload length ( + 1 byte to include the null termination)
      memcpy(&rf_packet[1], packet, rf_packet[0]);

      rf_packet_n = rf_packet[0] + 1;  // include the length byte in the total number of bytes to send
      
      rf_mode_tx = 1;
      RFTXRXIE=1;

      RFST = RFST_STX;
      
      RFIF &= ~RFIF_IM_DONE;      
}

void radio_init(void) {
/* RF settings SoC
  Base frequency = 891
 Carrier frequency = 891 
 Modulated = true 
 Modulation format = GFSK 
 Manchester enable = false 
 Sync word qualifier mode = 15/16 sync word bits detected, no carrier sense
 Preamble count = 2   
 Channel spacing = 199.951172 
 Carrier frequency = 914.999969 
 Data rate = 4.00448 
 RX filter BW = 58.035714 
  = Normal mode 
 Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 CRC enable = true 
 Packet length = 255 
 Device address = 0 
 Address config = No address check 
  = false 
 PA ramping = false 
 TX power = 10 

 */

ADDR      =     0x00;       // Device Address 
MCSM2     =     0x07;       // Main Radio Control State Machine Configuration 
MCSM1     =     0x30;       // Main Radio Control State Machine Configuration 
MCSM0     =     0x18;       // Main Radio Control State Machine Configuration 
FOCCFG    =     0x17;       // Frequency Offset Compensation Configuration 
BSCFG     =     0x6C;       // Bit Synchronization Configuration 
AGCCTRL2  =     0x03;       // AGC Control 
AGCCTRL1  =     0x40;       // AGC Control 
AGCCTRL0  =     0x91;       // AGC Control 
SYNC1     =     0xD3;       // Sync Word, High Byte 
SYNC0     =     0x91;       // Sync Word, Low Byte 
PKTLEN    =     RADIO_PAYLOAD_MAX;       // Packet Length 
PKTCTRL1  =     0x04;       // Packet Automation Control 
PKTCTRL0  =     0x04;       // Packet Automation Control 
TEST2     =     0x81;       // Various Test Settings 
TEST1     =     0x35;       // Various Test Settings 
TEST0     =     0x09;       // Various Test Settings 
PA_TABLE7 =     0x00;       // PA Power Setting 7 
PA_TABLE6 =     0x00;       // PA Power Setting 6 
PA_TABLE5 =     0x00;       // PA Power Setting 5 
CHANNR    =     0x00;       // Channel Number 
PA_TABLE4 =     0x00;       // PA Power Setting 4 
FSCTRL1   =     0x06;       // Frequency Synthesizer Control 
FSCTRL0   =     0x00;       // Frequency Synthesizer Control 
FREQ2     =     0x22;       // Frequency Control Word, High Byte 
FREQ1     =     0x44;       // Frequency Control Word, Middle Byte 
FREQ0     =     0xEC;       // Frequency Control Word, Low Byte 
MDMCFG4   =     0xF7;       // Modem configuration 
MDMCFG3   =     0x43;       // Modem Configuration 
FREND1    =     0x56;       // Front End RX Configuration 
FREND0    =     0x10;       // Front End TX Configuration 
FSCAL3    =     0xE9;       // Frequency Synthesizer Calibration 
FSCAL2    =     0x2A;       // Frequency Synthesizer Calibration 
FSCAL1    =     0x00;       // Frequency Synthesizer Calibration 
FSCAL0    =     0x1F;       // Frequency Synthesizer Calibration 
MDMCFG2   =     0x15;       // Modem Configuration 
MDMCFG1   =     0x02;       // Modem Configuration 
MDMCFG0   =     0xF8;       // Modem Configuration 
DEVIATN   =     0x24;       // Modem Deviation Setting 
PA_TABLE3 =     0x00;       // PA Power Setting 3 
PA_TABLE2 =     0x00;       // PA Power Setting 2 
PA_TABLE1 =     0x00;       // PA Power Setting 1 
PA_TABLE0 =     0xC2;       // PA Power Setting 0 
IOCFG2    =     0x00;       // Radio Test Signal Configuration (P1_7) 
IOCFG1    =     0x00;       // Radio Test Signal Configuration (P1_6) 
IOCFG0    =     0x00;       // Radio Test Signal Configuration (P1_5) 


  PKTCTRL0 |= 1<<0;     // variable length
  PKTCTRL1 |= 1<<2;     // append_status

  RFST = RFST_SIDLE;
  
  EA = 1;   // global interrupt enable

}
