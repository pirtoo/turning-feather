/* 
 * SD card setup
  */
#include "sdcard.h"

File checksdfile(const char *filename) {
  File file;

  if (!SD.exists(filename)) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist");
    return file;
  }
  // Open file for reading
  file=SD.open(filename, FILE_READ);
  if (!file) {
    lcd_print("Couldn't open ");
    lcd_println(filename);
  }
  return file;
}

File checkspiffsfile(const char *filename) {
  File file;

  if (!SPIFFS.exists(filename)) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist");
    return file;
  }
  // Open file for reading
  file=SPIFFS.open(filename, FILE_READ);
  if (!file) {
    lcd_print("Couldn't open ");
    lcd_println(filename);
  }
  return file;
}

void storage_init() {
  // SD card and spiffs setup
  uint8_t count=0;

  while (!SD.begin(SD_CS) and count <= SD_RETRIES) {
    delay(1000);
    // Wait for SD card, loop retrying.
    if (count > 2) {
      lcd_println("SD card init failed");
    }
    count++;
  }

  if (!SPIFFS.begin(true)) {
    lcd_println("SPIFFS storage:");
    lcd_println("  init failed");
  }
   
}

File turn_file_init() {
  // Config file load
  Serial.print(F("Reading config from: "));
  Serial.println(turnconf_file);
  // Open the file
  File conffile=checksdfile(turnconf_file);
  if (!conffile) {
    lcd_println("Can't read config from SD.");
    lcd_println("Trying SPIFFS:");
    conffile=checkspiffsfile(turnconf_file);
    if (!conffile) {
      lcd_println("File not found on SPIFFS");
      while (1);
    } else {
      lcd_println("Using backup file from");
      lcd_println("SPIFFS.");
      delay(5000);
    }
  }
  return conffile;
}
