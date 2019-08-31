/* 
 * SD card and config file 
  */

#ifndef TURN_SDCARD
#define TURN_SDCARD

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

File turn_file_init();

#endif //TURN_SDCARD
