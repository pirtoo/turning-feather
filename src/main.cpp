/*
 * Turning target controller.
 * Turning feather.
 *
 * Set up for PlatformIO
 */


// TODO: Put fudge times into a config file
// TODO: Put chirp (and chirp time) into config file
// TODO: WIFI/web server
//       - edit device config file
//       - ability to edit extra configs
// TODO: add extra turning configs, concat them from
//         main file, write out seperately?
// TODO: Have a directory on SD let you choose config?
//       - or boot with face/away held down to choose
//         files?
// TODO: Text for stage use, eg: "GRCF stage 1"?
//       - will this make the file too large for json?


// General includes
#include <Arduino.h>
#include "main.h"
#include "turning.h"
#include "turn_tft.h"
#include "zpt_serial.h"
#include "sdcard.h"
#include "ui/vars.h"

bool config_mode=false;
unsigned long start_millis;

#if defined(FEATHER_ESP32_V2) && defined(ESP_V2_NEOPIXEL)
#include "turn_neopixel.h"
#endif //FEATHER_ESP32_V2 && ESP_V2_NEOPIXEL

#ifdef ZPT_BUTTONS
#include "turn_zpt_buttons.h"
#endif //ZPT_BUTTONS

#ifdef PHYSICAL_BUTTONS
#include "analogbuttons.h"
#endif // PHYSICAL_BUTTONS

// WiFi
#ifdef TURN_WIFI_ENABLE
#include "turn_wifi.h"
#endif //TURN_WIFI_ENABLE

#ifdef USE_ZPT_SERIAL
#include "zpt_serial.h"
void zpt_serial_actions();
void zpt_serial_actions() {
  if (zpt_packet_lowbatt()) {
    // Print a red on white '!' if remote battery low
    // TODO display element or popup?
    //lcd_statusprint('!');
  } else if (!zpt_packet_learn()) {
    // Print a red on white '?' if remote not paired
    // TODO dispay element?
    //lcd_statusprint('?');
  } else {
    // Clear status
    // TODO clear element
    //lcd_statusclear();
  }
  set_var_zpt_signal_strength_int(zpt_packet_rssi());
}
#endif //USE_ZPT_SERIAL

void set_config_mode(bool mode) {
  config_mode=mode;
}

// Setup everything that needs to be set up
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

  // Serial setup
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    delay(100); // wait for serial port
  }
  // some delay before dealing with SPI, let it settle
  delay(1000);
  Serial.println("\r\nStart\r\n");
  Serial.println("Turning Feather");
  Serial.println("(c) pir 2019-2026");
  Serial.println("tf@pir.net\r\n");

#if defined(FEATHER_ESP32_V2) && defined(ESP_V2_NEOPIXEL)
  // Set up a Neopixel if configured.
  np_setup();
#endif //FEATHER_ESP32_V2 && ESP_V2_NEOPIXEL

  // If microsd doesn't work
  // Keep this before the display setup;
  //SPI.begin(SCK, MISO, MOSI, SD_CS_PIN);
  pinMode(SD_CS_PIN, OUTPUT);
  //SPI.setDataMode(SPI_MODE0);

#ifdef DEBUG
  Serial.println("Calling storage_init");
#endif //DEBUG
  // Keep this before the display setup
  storage_init();

#ifdef DEBUG
  Serial.println("Running setup_tft_screen");
#endif //DEBUG
  setup_tft_screen();

  //loadScreen(SCREEN_ID_SPLASH);
  //loop_tft_screen();

#ifdef USE_ZPT_SERIAL
#ifdef DEBUG
  Serial.println("Running zpt_serial_setup");
#endif //DEBUG
  // ZPT serial setup
  zpt_serial_setup();
#endif //USE_ZPT_SERIAL

  // Is the turn/face button being held down at boot?
  if (analogRead(BUTTONS_PIN) < 5) {
//  if (true) {
    set_config_mode(true);
    loadScreen(SCREEN_ID_CONFIG_MAIN);
  }

#ifdef DEBUG
  Serial.println("Running setup_turnconfig");
#endif //DEBUG
  // Load the config into a datastructure
  setup_turnconfig();

  // If enabled bring up WiFi
#ifdef TURN_WIFI_ENABLE
#ifdef DEBUG
  Serial.println("Calling initWifi");
#endif //DEBUG
  if (! initWifi()) {
    // WiFi init failed
    // TODO change this to a popup?
    lcd_println("WiFi disabled             ");
    delay(2000);
  }
#else //TURN_WIFI_ENABLE
  Serial.println("WiFi disabled, TURN_WIFI_ENABLE not defined");
#endif //TURN_WIFI_ENABLE

#ifdef PHYSICAL_BUTTONS
  // Physical buttons setup
#ifdef DEBUG
  Serial.println("Calling buttons_setup");
#endif //DEBUG
  buttons_setup();
#endif //PHYSICAL_BUTTONS

  // bravo/ZPT RF setup
#ifdef DEBUG
  Serial.println("Calling zpt_buttons_setup");
#endif //DEBUG
  zpt_buttons_setup();

  // Timer and face/away setup.
#ifdef DEBUG
  Serial.println("Calling timer_setup");
#endif //DEBUG
  timer_setup();

  Serial.println(F("\r\nSetup finished.\r\n"));
#if defined(FEATHER_ESP32_V2) && defined(ESP_V2_NEOPIXEL)
  np(0);
#endif //FEATHER_ESP32_V2 && ESP_V2_NEOPIXEL
  delay(2000);
}

void loop() {
  // Anything related to the screen/UI
  loop_tft_screen();

#ifdef TURN_WIFI_ENABLE
  wifiloop();
#endif //TURN_WIFI_ENABLE

  if (! config_mode) {
#ifdef ZPT_BUTTONS
    zpt_buttons_loop();
#endif // ZPT_BUTTONS

#ifdef PHYSICAL_BUTTONS
    // Physical buttons action
    buttons_loop();
#endif

    // Check semaphore and run turntick if ready
    turntick_loop();
  }

#ifdef USE_ZPT_SERIAL
  // ZPT serial handling
  zpt_serial_loop();
  if (zpt_packet_isready()) {
    zpt_serial_actions();
    // Signal ready for another packet.
    zpt_packet_getnew();
  }
#endif //USE_ZPT_SERIAL

  yield();
}
