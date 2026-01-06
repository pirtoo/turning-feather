/*
 * Turning target controller.
 * Turning feather.
 * 
 * General definitions.
 */

#ifndef TF_MAIN_H
#define TF_MAIN_H


// Extra debugging to the serial port while running
//#define DEBUG
//#define DEBUG2
//#define DEBUG_TIMER

// Using the V2 version of the Adafruit ESP32, default is V1
//#define HUZZAH32_V2
// If you have the ESP32 V2 you can use the built in NEOPIXEL
//#define ESP_V2_NEOPIXEL

// Using the V2 version of the Adafruit TFT 2.4" featherwing
//#define TFT_FW_24_V2

// To use ESP32 WiFi
//#define TURN_WIFI_ENABLE

// SPIFFS or LittleFS, default is SPIFFS
#define LITTLEFS

// Default config file, json
#define TURNCONF "/turnconf.txt"
// Default splash image, bmp
#define SPLASH_BMP "/pp1-320x240.bmp"

// Constants
// Clock rate for timer
#define CLOCK_RATE 100000
#define ONE_SECOND 100000
#define TENTH_SECOND 10000

// How long a start beep lasts
#define BEEP_LENGTH 0.8*ONE_SECOND
// How long to keep the motors going on a turn
#define CHANGE_LENGTH 0.4*ONE_SECOND
// Tenth of a second gives enough repeats to loop properly
#define TURN_RATE TENTH_SECOND

// Keep the face/away lines high while a stage is running
#define KEEP_CHANGE_ACTIVE

// Length of beep chirp when RF stage change happens
// Comment out to disable entirely
#define STAGE_CHANGE_CHIRP 0.4*TENTH_SECOND

// Fudge to allow time for targets to turn
// in 10ths of a second
#define FACE_FUDGE 3
#define AWAY_FUDGE 3

// Number of tenths of seconds (TURN_RATE) from start
// until the targets are faced or the beep, depending
// on config.
#define INIT_PAUSE 6


// LED_BUILTIN is 13 on the HUZZAH32
#define BUZZER LED_BUILTIN

#define FACE_PIN 25
#define AWAY_PIN 26

#define RF_1 4
#ifdef HUZZAH32_V2
#define RF_2 37
#else
#define RF_2 21
#endif //HUZZAH32_V2
#define RF_3 27
#define RF_4 34

// Physical buttons pin
#define BUTTONS_PIN 39 // A3

// On Feather ESP32 V2 the physical board button is BUTTON/38

// Unused currently
// Double the value here to get battery voltage
//#define BATTERY_VOLTAGE A13

// Unused output on the HUZZAH32
#define UNUSED_1 12 // AKA A11, wired to 5v MOSFET
//#define UNUSED_2 A4/36

// Using these to generate rising edge interrupts fails badly.
//#define BAD_1 36
//#define BAD_2 39

// Serial speed
#define SERIAL_SPEED 115200

// Extra serial output from the ZPT
#define USE_ZPT_SERIAL
// Enable buttons on RF remote
#define RF_BUTTONS
// Enable physical buttons on case
#define PHYSICAL_BUTTONS

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

// For core 3.0.0 and higher
#if ESP_ARDUINO_VERSION_MAJOR >= 3 && defined(ESP32)
  #include <driver/gpio.h>
  #include <rom/ets_sys.h>
#endif //ESP_ARDUINO_VERSION_MAJOR >= 3 && defined(ESP32)

#endif //TF_MAIN_H