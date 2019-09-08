/* 
 * SD card and config file 
  */

#ifndef TURNING_SDCARD_H
#define TURNING_SDCARD_H

#include <SPI.h>
#include <SD.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include <SPIFFS.h>

void storage_init();
File turn_file_init();

#endif //TURNING_SDCARD_H
