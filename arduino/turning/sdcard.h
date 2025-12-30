/* 
 * SD card and config file 
  */

#ifndef TURNING_SDCARD_H
#define TURNING_SDCARD_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef LITTLEFS
#include <esp_littlefs.h>
#include <LittleFS.h>
#else
#include <SPIFFS.h>
#endif // LITTLEFS

void storage_init();
File turn_file_init();

bool sd_init_ok=false, localfs_init_ok=false;

#endif //TURNING_SDCARD_H
