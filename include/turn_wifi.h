/*
 * Wifi utils for TurningFeather
 */

#include "main.h"

#ifdef TURN_WIFI_ENABLE
#ifndef TF_WIFI_H
#define TURN_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
#include "LittleFS.h"

#define TURN_WIFI_PREFS "TFWifi"

#ifndef TURN_WIFI_DEFAULT_AP
#define TURN_WIFI_DEFAULT_AP "TurningFeather"
#endif //TURN_WIFI_DEFAULT_AP

#ifndef TURN_WIFI_DEFAULT_ADMIN
#define TURN_WIFI_DEFAULT_ADMIN "admin"
#endif //TURN_WIFI_DEFAULT_ADMIN

#define TURN_WIFI_PASS_MIN 12

// Debug HTTP POST requests
//#define DEBUG_POST

// Initialise wifi in setup()
bool initWifi(void);
// Set all wifi settings back to defaults
void resetWifi(void);
// Tasks to do in the loop
void wifiloop(void);

#endif //TF_WIFI_H
#endif //TURN_WIFI_ENABLE