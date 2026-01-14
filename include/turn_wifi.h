/*
 * Wifi utils for TurningFeather
 */

#include "main.h"

#ifdef TURN_WIFI_ENABLE
#ifndef TURN_WIFI_H
#define TURN_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
#include "LittleFS.h"

#define TURN_WIFI_PREFS "TFWifi"
#define TURN_WIFI_DEFAULT_AP "TurningFeather"
#define TURN_WIFI_PASS_MIN 12

// Debug HTTP POST requests
//#define DEBUG_POST

// Initialise wifi in setup()
void initWifi(void);
// Set all wifi settings back to defaults
void resetWifi(void);
// Tasks to do in the loop
void wifiloop(void);

#endif //IF_WIFI_H
#endif //TURN_WIFI_ENABLE