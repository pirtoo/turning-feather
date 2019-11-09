/*
 * Utilities
 */

#ifndef TURNING_UTILS_H
#define TURNING_UTILS_H

#define STR_IMPL_(x) #x
#define STR(x) STR_IMPL_(x)

// ESP specific
#ifdef ESP32
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
void do_restart();
#endif //ESP32

#endif //TURNING_UTILS_H
