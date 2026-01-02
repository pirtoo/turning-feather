/*
 * Read serial input from the ZPT if available to get low battery
 * Status, signal strength, etc.
 * Also possible to get button presses to detect concurrent buttons
 * and other things.
 * Packet and serial IO type defined in ZPT datasheet available from:
 * https://www.rfsolutions.co.uk/
 */

#ifdef USE_ZPT_SERIAL

#include "zpt_serial.h"

uint8_t c=0;

uint32_t zpt_packet_serialnum(const struct zpt_serial_packet *packet) {
  // Assemble the serial number array into one number
  return packet->serialnum[2] +
         (packet->serialnum[1]<<8) +
         (packet->serialnum[0]<<16);
}

bool zpt_packet_lowbatt(const struct zpt_serial_packet *packet) {
  // Extract low battery state
  return bitRead(packet->learn_lowbatt, 0) == 1;
}

bool zpt_packet_learn(const struct zpt_serial_packet *packet) {
  // Extract learned button state
  return bitRead(packet->learn_lowbatt, 1) == 1;
}

void zpt_serial_setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
}


void zpt_serial_loop() {

  while (!zpt_serialpacket_ready and Serial2.available()) {
    // Read bytes into the packet struct via pointer
    // to assemble a full packet and make it available.
#ifdef DEBUG2
    Serial.println("Getting ZPT_SERIAL data");
#endif

    Serial2.readBytes((uint8_t *) &packetin_p[c], sizeof(uint8_t));
    if (packetin_p[c] == '\n') {
      // We got a newline
      if (c=sizeof(packetin) -1) {
        // Newline in the right place, we should have a valid packet
        zpt_serialpacket_ready=true;
      }
      // Reset to the start for next packet
      c=0;
    } else if (c <= sizeof(packetin)) {
      // Next byte in the packet
      c++;
    } else {
      // We went over length, reset to start, not valid packet
      c=0;
    }
  }
  
#ifdef DEBUG2
  if (zpt_serialpacket_ready) {
    // Print available packet.
    Serial.print("Serialnum=");
    Serial.println(zpt_packet_serialnum(&packetin));
    Serial.print("inputs_low=");
    Serial.println(packetin.inputs_low);
    Serial.print("inputs_high=");
    Serial.println(packetin.inputs_high);
    Serial.print("tx_battery=");
    Serial.println(packetin.tx_battery);
    Serial.print("lowbatt=");
    Serial.println(zpt_packet_lowbatt(&packetin) ? 'Y' : 'N');
    Serial.print("learn=");
    Serial.println(zpt_packet_learn(&packetin) ? 'Y' : 'N');
    Serial.print("rssi=");
    Serial.println(packetin.rssi);
    Serial.println("");

    // Signal ready for another packet.
    // Should be done after using the packet
    //zpt_serialpacket_ready=false;
  }
#endif
}
#endif //USE_ZPT_SERIAL
