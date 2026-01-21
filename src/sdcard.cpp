/*
 * SD card setup
 */
#include "turning.h"
#include "turn_tft.h"
#include "sdcard.h"

bool sd_init_ok=false, localfs_init_ok=false;

File checksdfile(const char *filename) {
  File file;

  if (!SD.exists(filename)) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist on SD");
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
  File file = LittleFS.open(filename);
  if (!file || file.isDirectory()) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist on internal filesystem");
  }
  return file;
}

void storage_init(void) {
  // SD card and littlefs setup
  uint8_t count=0;

#ifdef DEBUG
  Serial.println("Starting storage_init");
#endif //DEBUG
  while (! SD.begin(SD_CS_PIN) and count <= SD_RETRIES) {
#ifdef DEBUG2
    Serial.println("storage_init SD.begin loop");
#endif //DEBUG
    delay(500);
    count++;
  }
#ifdef DEBUG
  Serial.println("SD init done");
#endif //DEBUG

  if (count <= SD_RETRIES) {
    sd_init_ok=true;
    Serial.println("SD Init OK");
  } else {
    lcd_println("SD card init failed");
    Serial.print("Trying to connect to SD on pin ");
    Serial.println(SD_CS_PIN);
  }

  if (!LittleFS.begin(true)) {
    lcd_println("LittleFS storage:");
    lcd_println("  mount failed");
  } else {
    localfs_init_ok=true;
  }
}

File turn_file_init(const char *turnconf_file) {
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
      while (1) {
        delay(100);
      }
    } else {
      lcd_println("Using backup file from internal fs.");
      delay(4000);
    }
  }
  return conffile;
}
