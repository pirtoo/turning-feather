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

File checkfsfile(const char *filename) {
#ifdef LITTLEFS
  File file = LittleFS.open(filename);
#else
  File file = SPIFFS.open(filename);
#endif // LITTLEFS
  if (!file || file.isDirectory()) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist on internal filesystem");
  }
  return file;
}

void storage_init() {
  // SD card and spiffs or littlefs setup
  uint8_t count=0;

  while (!SD.begin(SD_CS) and count <= SD_RETRIES) {
    delay(500);
    count++;
  }

  if (count <= SD_RETRIES) {
    sd_init_ok=true;
  } else {
    lcd_println("SD card init failed");
  }

#ifdef LITTLEFS
  if (!LittleFS.begin(true)) {
    lcd_println("LittleFS storage:");
    lcd_println("  mount failed");    
  } else {
    localfs_init_ok=true;
  }
#else
  if (!SPIFFS.begin(true)) {
    lcd_println("SPIFFS storage:");
    lcd_println("  init failed");
  } else {
    localfs_init_ok=true;
  }
#endif // LITTLEFS
}

File turn_file_init() {
  // Config file load
  Serial.print(F("Reading config from: "));
  Serial.println(turnconf_file);
  // Open the file
  File conffile=checksdfile(turnconf_file);
  if (!conffile) {
    lcd_println("Can't read config from SD.");
    conffile=checkfsfile(turnconf_file);
    if (!conffile) {
      lcd_println("File not found on internal fs");
      while (1);
    } else {
      lcd_println("Using backup file from internal fs.");
      delay(5000);
    }
  }
  return conffile;
}
