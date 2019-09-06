/* 
 * SD card and config file 
  */

#ifndef TURN_SDCARD
#define TURN_SDCARD

#include <SPI.h>
#include <SD.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include <SPIFFS.h>

void storage_init();
File turn_file_init();

#endif //TURN_SDCARD
