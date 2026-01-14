/* 
 * SD card setup
 */
#include "turning.h"
#include "turn_lcd.h"
#include "sdcard.h"

bool sd_init_ok=false, localfs_init_ok=false;

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
  File file = LittleFS.open(filename);
  if (!file || file.isDirectory()) {
    lcd_print("Config file ");
    lcd_print(filename);
    lcd_println(" does not exist on internal filesystem");
  }
  return file;
}

void storage_init() {
  // SD card and littlefs setup
  uint8_t count=0;

  while (!SD.begin(SD_CS_PIN) and count <= SD_RETRIES) {
    delay(500);
    count++;
  }

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
      while (1);
    } else {
      lcd_println("Using backup file from internal fs.");
      delay(5000);
    }
  }
  return conffile;
}
