/*
 * Wifi utils for TurningFeather
 */

#ifndef TF_WIFI_H
#define TF_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
#include "LittleFS.h"

#define TF_WIFI_PREFS "TFWifi"
#define TF_WIFI_DEFAULT_AP "TurningFeather"
#define TF_WIFI_PASS_MIN 12

// Debug HTTP POST requests
//#define DEBUG_POST

// Initialise wifi in setup()
void initWifi(void);
// Set wifi back to defaults
void resetWifi(void);

#endif //IF_WIFI_H