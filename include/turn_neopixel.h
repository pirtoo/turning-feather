/*
 * If we have a Neopixel we can use it to illuminate stage changes.
 */

#if defined(FEATHER_ESP32_V2) && defined(ESP_V2_NEOPIXEL)
#ifndef TURN_NEOPIXEL_H
#define TURN_NEOPIXEL_H

#include <Arduino.h>

#ifndef ESP_V2_NEOPIXEL_BRIGHTNESS
#define ESP_V2_NEOPIXEL_BRIGHTNESS 128
#endif //ESP_V2_NEOPIXEL_BRIGHTNESS

void np_setup();
void np(uint32_t c);

#endif //TURN_NEOPIXEL_H
#endif //FEATHER_ESP32_V2 && ESP_V2_NEOPIXEL
