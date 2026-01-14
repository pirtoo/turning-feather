/* 
 * SD card and config file 
 */

#ifndef TF_SDCARD_H
#define TF_SDCARD_H

#include "turning.h"

// 33 on the logger featherwing
//#define SD_CS_PIN SS

#ifndef SD_CS_PIN
// Pin for SD on 2.4" TFT featherwing V1 and V2
#define SD_CS_PIN 14
#endif //SD_CS_PIN

#ifndef SD_RETRIES
// Number of times to retry the SD card
#define SD_RETRIES 2
#endif //SD_RETRIES

#include <SPI.h>
#include <SD.h>
#include <esp_littlefs.h>
#include <LittleFS.h>

void storage_init();
File turn_file_init(const char *turnconf_file);

#endif //TF_SDCARD_H
