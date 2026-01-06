/*
 * Turning target controller.
 * Turning feather.
 * 
 * Set up for PlatformIO
 */


// TODO: Put fudge times into a config file
// TODO: Put debug settings into a config file
// TODO: Put chirp (and chirp time) into config file
// TODO: WIFI/web server
//       - edit device config file
//       - watch current state, provide buttons
//       - ability to edit extra configs
// TODO: add extra turning configs, concat them from
//         main file, write out seperately?
// TODO: Have a directory on SD let you choose config?
//       - or boot with face/away held down to choose
//         files?
// TODO: Text for stage use, eg: "GRCF stage 1"?
//       - will this make the file too large for json?
// TODO: Mode to show ZPT signal strength

// General includes
#include <Arduino.h>
#include "main.h"
#include "turning.h"
#include "turn_lcd.h"
#include "zpt_serial.h"

#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
#include "turn_neopixel.h"
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL

#ifdef USE_ZPT_SERIAL
#include "zpt_serial.h"
#endif //USE_ZPT_SERIAL

// WiFi
#ifdef TURN_WIFI_ENABLE
#include "turn_wifi.h"
#endif //TURN_WIFI_ENABLE

// SD card and config file
#include "sdcard.h"

// Adafruit featherwing LCD display
#include "turn_lcd.h"
uint8_t lcd_button_num=0;

/*
 * Analog multi buttons
 */
// TODO move to their own file
#ifdef PHYSICAL_BUTTONS
#include <AnalogMultiButton.h>

const int buttons_values[2] = {5, 684};
AnalogMultiButton buttons(BUTTONS_PIN, 2, buttons_values);

void buttons_setup() {
  // Make sure all analog systems read the same
  analogReadResolution(10);
}

void buttons_loop() {
  buttons.update();
  if (buttons.onPress(0)) {
#ifdef DEBUG2
    Serial.println("Physical 0 has been pressed");
#endif //DEBUG2
    button_action(2, false);
  }
  if (buttons.onPress(1)) {
#ifdef DEBUG2
    Serial.println("Physical 1 has been pressed");
#endif //DEBUG2
    button_action(1, false);
  }
}
#endif // PHYSICAL_BUTTONS


#ifdef USE_ZPT_SERIAL
void rf_serial_actions() {
  if (zpt_packet_lowbatt()) {
    // Print a red on white '!' if remote battery low
    lcd_statusprint('!');
  } else if (!zpt_packet_learn()) {
    // Print a red on white '?' if remote not paired
    lcd_statusprint('?');
  } else {
    // Clear status
    lcd_statusclear();
  }
}
#endif //USE_ZPT_SERIAL

/*
 * Setup and loop
 */
void setup() {
  // outputs setup
  // Make aure the 12v mosfet drivers are off
  pinMode(FACE_PIN, OUTPUT);
  digitalWrite(FACE_PIN, LOW);
  pinMode(AWAY_PIN, OUTPUT);
  digitalWrite(AWAY_PIN, LOW);
  // Set up other general pins
  pinMode(BUZZER, OUTPUT);
  pinMode(UNUSED_1, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(UNUSED_1, LOW);
  pinMode(BUTTONS_PIN, INPUT);


  // Serial setup
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    delay(100); // wait for serial port
  }
  delay(1000);
#ifdef DEBUG
  Serial.println("\r\nStart\r\n");
#endif //DEBUG

#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
  // Set up a Neopixel if configured.
  np_setup();
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL

  // Keep this before the lcd_setup();
  //SPI.begin(SCK, MISO, MOSI, SD_CS_PIN);
  //pinMode(SD_CS_PIN, OUTPUT);
  //SPI.setDataMode(SPI_MODE0);

#ifdef DEBUG
  Serial.println("Calling lcd_setup");
#endif //DEBUG
  // Basic LCD setup
  lcd_setup();

#ifdef DEBUG
  Serial.println("Calling storage_init");
#endif //DEBUG
  storage_init();

#ifdef DEBUG
  Serial.println("Calling lcd_splash");
#endif //DEBUG
  lcd_splash(SPLASH_BMP);
  Serial.println("\r\n");
  lcd_println("Turning feather controller");
  lcd_println("(c) pir 2019-2026         ");
  lcd_println("tf@pir.net                ");
  Serial.println("");

  delay(3000);

  // Load the config into a datastructure
  setup_turnconfig();

#ifdef PHYSICAL_BUTTONS
  // Physical buttons setup
  buttons_setup();
#endif //PHYSICAL_BUTTONS

  // bravo/ZPT RF setup
  rf_buttons_setup();

#ifdef USE_ZPT_SERIAL
  // ZPT serial setup
  zpt_serial_setup();
#endif //USE_ZPT_SERIAL

  timer_setup();
  // Timer and face/away setup.

  // If enabled bring up WiFi
#ifdef TURN_WIFI_ENABLE
  initWifi();
#endif //TURN_WIFI_ENABLE


  Serial.println(F("\r\nSetup finished.\r\n"));
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
  np(0);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
}

void loop() {
  // TODO move this to an rf buttons file
#ifdef RF_BUTTONS
  rf_buttons_loop();
#endif // RF_BUTTONS
  // Check semaphore and run turntick if ready
  turntick_loop();
  // Take screen button press actions
  button_action(lcd_button(), false);

#ifdef PHYSICAL_BUTTONS
  // Physical buttons action
  buttons_loop();
#endif

#ifdef USE_ZPT_SERIAL
  // ZPT serial handling
  zpt_serial_loop();
  if (zpt_packet_isready()) {
    rf_serial_actions();
    // Signal ready for another packet.
    zpt_packet_getnew();
  }
#endif //USE_ZPT_SERIAL

  yield();
}
