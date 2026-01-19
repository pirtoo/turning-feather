/*
 * If we have a Neopixel we can use it to illuminate stage changes.
 */

// Only valid on the V2 and if enabled.
#if defined(FEATHER_ESP32_V2) && defined(ESP_V2_NEOPIXEL)
#include <turn_neopixel.h>
#include <Adafruit_NeoPixel.h>

// V2 ESP32 Feather has one built in Neopixel
Adafruit_NeoPixel strip(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void np_setup() {
  Serial.println("Setting up Neopixel");
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.setBrightness(ESP_V2_NEOPIXEL_BRIGHTNESS);
  np(0xFFFFFF);
}

void np(uint32_t c) {
  if (c==0)
    strip.clear();
  else
    strip.setPixelColor(0, c);
  strip.show();
}
#endif //FEATHER_ESP32_V2 && ESP_V2_NEOPIXEL
