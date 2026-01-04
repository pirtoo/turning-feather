/*
 * Read serial input from the ZPT if available to get low battery
 * Status, signal strength, etc.
 * Also possible to get button presses to detect concurrent buttons
 * and other things.
 * Packet and serial IO type defined here:
 * https://www.rfsolutions.co.uk/downloads/1523289764DS-ZPT-1.pdf
 */

#ifdef USE_ZPT_SERIAL
#ifndef ZPT_SERIAL_H
#define ZPT_SERIAL_H

// HUZZAH32 defaults for TX/RX pins for Serial2.
#ifndef RXD2
#ifdef RX
#define RXD2 RX
#elif defined(RX1)
#define RXD2 RX1
#else
#define RXD2 16
#endif //RX
#endif //RXD32

#ifndef TXD2
#ifdef RX
#define TXD2 TX
#elif defined(TX1)
#define TXD2 TX1
#else
#define TXD2 17
#endif //RX
#endif //RXD32

//#define RXD2 RX
//#define TXD2 TX

struct zpt_serial_packet {
  // Serial number of the transmitter, largest byte first
  uint8_t serialnum[3];
  // Inputs 0-8 state in binary
  uint8_t inputs_low;
  // Inputs 9-16 state in binary
  uint8_t inputs_high;
  // Transmitter battery level if available
  uint8_t tx_battery;
  // bit0=1 when battery is low
  // bit1=1 when button has been learned
  uint8_t learn_lowbatt;
  // Received Signal Strength Indicator
  uint8_t rssi;
  // Terminating CR/LF
  uint8_t cr;
  uint8_t lf;
};

uint32_t zpt_packet_serialnum(void);
bool zpt_packet_lowbatt(void);
bool zpt_packet_learn(void);
bool zpt_packet_isready(void);
void zpt_packet_getnew(void);
// include in setup()
void zpt_serial_setup(void);
// include in loop() and then use the packet contents via above
void zpt_serial_loop(void);


#endif // ZPT_SERIAL_H
#endif // USE_ZPT_SERIAL
