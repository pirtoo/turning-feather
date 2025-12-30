/*
 * Utilities
 */

#ifndef TURNING_UTILS_H
#define TURNING_UTILS_H

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

// ESP specific
#ifdef ESP32
#include <esp_private/esp_int_wdt.h>
#include <esp_task_wdt.h>
void do_restart();

// For core 3.0.0 and higher
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  #include <driver/gpio.h>
  #include <rom/ets_sys.h>
#endif
#endif //ESP32


#endif //TURNING_UTILS_H
