/*
 * Read serial input from the ZPT if available to get low battery
 * Status, signal strength, etc.
 * Also possible to get button presses to detect concurrent buttons
 * and other things.
 *  Packet and serial IO type defined here:
 * https://www.rfsolutions.co.uk/downloads/1523289764DS-ZPT-1.pdf
 */

#ifdef USE_ZPT_SERIAL
#ifndef ZPT_SERIAL_H
#define ZPT_SERIAL_H

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

struct zpt_serial_packet packetin;
uint8_t *packetin_p=(uint8_t *)&packetin;
bool zpt_serialpacket_ready=false, zpt_serial_setup_done=false;

uint32_t zpt_packet_serialnum(const struct zpt_serial_packet *packet);
bool zpt_packet_lowbatt(const struct zpt_serial_packet *packet);
bool zpt_packet_learn(const struct zpt_serial_packet *packet);
void zpt_serial_loop();

#endif // ZPT_SERIAL_H
#endif // USE_ZPT_SERIAL
