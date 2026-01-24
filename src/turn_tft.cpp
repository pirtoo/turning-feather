/*
 * Write to the featherwing touchscreen LCD and get
 * touch info
 */

#include <Arduino.h>
#include "SPI.h"
#include <Wire.h> // this is needed even though we aren't using it directly

#include "turn_tft.h"
#include "TurnConfig.h"
#include "zpt_serial.h"

#include "ui/vars.h"
#include "ui/screens.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Graphics display driver
Adafruit_ILI9341 tft=Adafruit_ILI9341(TFT_CS, TFT_DC);

// Touchscreen driver changes depending on V1 or V2 of Featherwing display
#if TSC2007_TS
// This is the V2 version of the feartherwing
#include <Adafruit_TSC2007.h>
// If you're using the TSC2007 there is no CS pin needed, so instead its an IRQ
#define TSC_IRQ STMPE_CS
// V2 display touch contoller
Adafruit_TSC2007 ts=Adafruit_TSC2007();
#else
// This is the V1 version of the featherwing
#include <Adafruit_STMPE610.h>
Adafruit_STMPE610 ts=Adafruit_STMPE610(STMPE_CS);
#endif //TSC2007_TS

// Glue between the device and LVGL
Adafruit_LvGL_Glue glue;

// snprintf buffer
#define BUFF_LEN 500
char buff[BUFF_LEN];

// Main tables to fill in data when running
lv_obj_t *table1, *table2;

// Text to be used on main screen buttons
const char *stop_str="STOPPED";
const char *run_str="RUNNING";
const char *face_str="FACE";
const char *away_str="AWAY";

void extra_lvgl_setup(void);

uint16_t read16(fs::File &f);
uint32_t read32(fs::File &f);


void extra_lvgl_setup() {
  // Setup some things that cannot be done through EEZ

  // Main tables in programming screen for stage numbers
  table1=objects.prog_table1;
  table2=objects.prog_table2;
  // Nonstandard font
  //lv_obj_set_style_text_font(table, &lv_font_unscii_8, LV_PART_MAIN | LV_STATE_DEFAULT);

  // table size in row/columns
  lv_table_set_row_cnt(table1, 3);
  lv_table_set_col_cnt(table1, 6);
  lv_table_set_row_cnt(table2, 3);
  lv_table_set_col_cnt(table2, 6);

  // Gap at the left hand side to centre things
  lv_table_set_col_width(table1, 0, 20);
  lv_table_set_col_width(table2, 0, 20);
  for (int i=1; i<5; i++) {
    lv_table_set_col_width(table1, i, PC_CWIDTH);
    lv_table_set_col_width(table2, i, PC_CWIDTH);
  }

  lv_table_set_cell_value(table1, PC_R_H1, PC_C_BEEP, "Beep");
  lv_table_set_cell_value(table1, PC_R_H1, PC_C_FACE, "Face");
  lv_table_set_cell_value(table1, PC_R_H1, PC_C_AWAY, "Away");
  lv_table_set_cell_value(table1, PC_R_H1, PC_C_REPEAT, "Repeat");

  lv_table_set_cell_value(table2, PC_R_H1, PC_C_FLASH, "Flash");
  lv_table_set_cell_value(table2, PC_R_H1, PC_C_FAWAY, "F Away");
  lv_table_set_cell_value(table2, PC_R_H1, PC_C_NAWAY, "N Away");
  lv_table_set_cell_value(table2, PC_R_H1, PC_C_AUTON, "Auto N");

  //lv_table_set_cell_value(table1, PC_R_S1, 0, "888.0");
  //lv_table_set_cell_value(table1, PC_R_S1, 1, "120.0");
  //lv_table_set_cell_value(table1, PC_R_S1, 2, "108.0");
  //lv_table_set_cell_value(table1, PC_R_S1, 3, "998.0");

  //lv_table_set_cell_value(table2, PC_R_S1, 0, "428.0");
  //lv_table_set_cell_value(table2, PC_R_S1, 1, "999.9");
  //lv_table_set_cell_value(table2, PC_R_S1, 2, "505.3");
  //lv_table_set_cell_value(table2, PC_R_S1, 3, "No");
}

void setup_tft_screen() {
  // Initialize display and touchscreen BEFORE glue setup
  delay(10);

#ifdef DEBUG
  Serial.println("Start display init");
#endif //DEBUG
  tft.begin();
  // Puts the USB port to the lower/right.
  tft.setRotation(3);
  //tft.setTextWrap(true);
  //tft.cp437(true);
  tft_splash(SPLASH_BMP);
#ifdef DEBUG2
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  Serial.print("Screen width: ");
  Serial.println(tft.width());
  Serial.print("Screen height: ");
  Serial.println(tft.height());
#endif //DEBUG2
#ifdef DEBUG
  Serial.println("Display init done");
#endif //DEBUG

#ifdef DEBUG
  Serial.println("Starting touchscreen controller init");
#endif //DEBUG

  LvGLStatus status=LVGL_OK;
#if TSC2007_TS
  if (ts.begin(0x48, &Wire)) {
      pinMode(TSC_IRQ, INPUT);
    // Initialize glue, passing in address of display & touchscreen
    status = glue.begin(&tft, &ts, TSC_IRQ);
#ifdef DEBUG
  Serial.println("Touchscreen controller TSC2007 OK");
#endif //DEBUG
#else //TSC2007_TS
  if (ts.begin()) {
    // Initialize glue, passing in address of display & touchscreen
    status = glue.begin(&tft, &ts);
#ifdef DEBUG
    Serial.println("Touchscreen controller STMPE610H OK");
#endif //DEBUG
#endif //TSC2007_TS
  } else {
    Serial.println("Touchscreen controller init fail");
  }

  if (status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
  }

#ifdef DEBUG
  Serial.println("EEZ UI init starting");
#endif //DEBUG
  // Initialise EEZ UI
  ui_init();

  extra_lvgl_setup();

  // Make sure the UI is updated
#ifndef EEZ_MANUAL_UI_TICK
  lv_timer_t *tick_timer = lv_timer_create(
  [](lv_timer_t *timer) {
    ui_tick();
  }, 5, NULL);
#endif //EEZ_MANUAL_UI_TICK
#ifdef DEBUG
  Serial.println("EEZ UI init done");
#endif //DEBUG

#ifdef DEBUG
  Serial.println("Display/touchscreen setup done");
#endif //DEBUG
}

void loop_tft_screen(void) {
  // EEZ UI tick can be in another task in FreeRTOS
#ifdef EEZ_MANUAL_UI_TICK
  ui_tick();
#endif //EEZ_MANUAL_UI_TICK

  // On ESP32, LittleVGL's task handler (`lv_task_handler`) is a task in
  // FreeRTOS and is pinned to the core upon successful initialization.
  // This means that you do not need to call `lv_task_handler()` within the loop() on ESP32
#if defined(ESP32) && ! defined(USE_LVGL_TASK)
  lv_task_handler();
#endif //ESP32 && ! USE_LVGL_TASK

/*
#ifdef EEZ_MANUAL_UI_TICK
  ui_tick();
#endif //EEZ_MANUAL_UI_TICK

#if defined(ESP32) && ! defined(USE_LVGL_TASK)
  lv_task_handler();
#endif //ESP32 && ! USE_LVGL_TASK
*/
}

void tft_face(const bool isface, const uint8_t in_stage) {
  // clear previous number
  tft_stagerun_clear(in_stage);
  // Set face/away
  if (isface) {
    // Change the colour of the run bar and the face/away button
    set_var_prog_face_bar_int(0);
    set_var_face_away_btn_checked_bool(true);
    set_var_face_away(face_str);
  } else {
    // Change the colour of the run bar and the face/away button
    set_var_prog_face_bar_int(0);
    set_var_face_away_btn_checked_bool(false);
    set_var_face_away(away_str);
  }
}

void tft_stop(const bool isstop) {
  // Set start/stop
  // If we're stopping or starting then the times can be cleared
  tft_stagerun_clear(IN_INIT);
  if (isstop) {
    // Change the colour of the face/away button
    set_var_start_stop_btn_checked_bool(false);
    // Change the text on the face/away button
    set_var_start_stop(stop_str);
  } else {
    // Change the colour of the face/away button
    set_var_start_stop_btn_checked_bool(true);
    // Change the text on the face/away button
    set_var_start_stop(run_str);
  }
}

void tft_prog_list(TurnConfig *tc) {
  // Update the selection list of programs
  char line[TURN_NAME_LENGTH +3];

  // Last line needs to not have a newline at the end
  snprintf(buff, sizeof(line), "%s", tc->program[0].longname);
  for (uint8_t i=1; i<tc->programs; i++) {
    snprintf(line, sizeof(line), "\n%s", tc->program[i].longname);
    strncat(buff, line, BUFF_LEN -1);
  }
  set_var_prog_list(buff);
}

void tft_table_cell_value_float(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const float value) {
  snprintf(buff, BUFF_LEN -1, "%4.1f", value);
  lv_table_set_cell_value(table, row, col, buff);
}

void tft_table_cell_value_int(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const u_int8_t value) {
  snprintf(buff, BUFF_LEN -1, "%d", value);
  lv_table_set_cell_value(table, row, col, buff);
}

void tft_table_cell_value_bool(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const bool value) {
  snprintf(buff, BUFF_LEN -1, "%s", value ? "Yes" : "No");
  lv_table_set_cell_value(table, row, col, buff);
}

void tft_stage_list(const struct StageConfig *stage,
                    const u_int8_t stages, const uint8_t stagenum) {
  // Update the selection list of stages
  char line[10];  // Stage 999
  // What the display is currently set to
  static uint8_t stages_now=0;

  // Set the list of stages to the right length
  // Last line needs to not have a newline at the end
  if (stages != stages_now) {
    snprintf(buff, sizeof(line), "%d", 1);
    if (stages > 1) {
      for (uint8_t i=2; i<=stages; i++) {
        snprintf(line, sizeof(line), "\n%d", i);
        strncat(buff, line, BUFF_LEN -1);
      }
    }
    set_var_stage_list(buff);
  }
  // Set the selected stage
  tft_stage(stage, stagenum);
}

void tft_prog(const uint8_t prognum) {
  // switch to a program number in the list
  set_var_prog_list_sel_int(prognum);
}

void tft_stage(const struct StageConfig *stage, const uint8_t stagenum) {
  // Display stage values on screen
  set_var_stage_list_sel_int(stagenum);

  tft_table_cell_value_float(table1, PC_R_S1, PC_C_BEEP, stage->beep / TIME_DIV);
  tft_table_cell_value_float(table1, PC_R_S1, PC_C_FACE, stage->face / TIME_DIV);
  tft_table_cell_value_float(table1, PC_R_S1, PC_C_AWAY, stage->away / TIME_DIV);
  tft_table_cell_value_int(table1, PC_R_S1, PC_C_REPEAT, stage->repeat);

  tft_table_cell_value_float(table2, PC_R_S1, PC_C_FLASH, stage->flash / TIME_DIV);
  tft_table_cell_value_float(table2, PC_R_S1, PC_C_FAWAY, stage->flashaway / TIME_DIV);
  tft_table_cell_value_float(table2, PC_R_S1, PC_C_NAWAY, stage->nextaway / TIME_DIV);
  tft_table_cell_value_bool(table2, PC_R_S1, PC_C_AUTON, stage->autonext);
}

void tft_stagerun(const struct ControlStatus *status, const uint16_t num, const uint8_t pos, const uint8_t repeat) {
  // Update the TFT display while a stage runs

  // Update the two run percentage bars
#ifdef DEBUG2
  Serial.printf("Setting face bar value to %d\r\n",status->faceperc);
  Serial.printf("Setting stage bar value to %d\r\n",status->stageperc);
#endif //DEBUG2
  set_var_prog_face_bar_int(status->faceperc);
  set_var_prog_stage_bar_int(status->stageperc);

  if (repeat == 0) {
    // Clear the repeat field
    if (pos == IN_INIT)
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_REPEAT, "");
  } else {
    tft_table_cell_value_int(table1, PC_R_D1, PC_C_REPEAT, repeat);
  }

  // Keep running numbers up to date
  switch (pos) {
    case IN_INIT:
      break;

    case IN_INIT_PAUSE:
      break;

    case IN_BEEP:
      // beep
      tft_table_cell_value_float(table1, PC_R_D1, PC_C_BEEP, num / TIME_DIV);
      break;

    case IN_FACE:
      // face
      tft_table_cell_value_float(table1, PC_R_D1, PC_C_FACE, num / TIME_DIV);
      break;

    case IN_AWAY:
      // away
      tft_table_cell_value_float(table1, PC_R_D1, PC_C_AWAY, num / TIME_DIV);
      break;

    case IN_FLASH:
      // flash
      tft_table_cell_value_float(table2, PC_R_D1, PC_C_FLASH, num / TIME_DIV);
      break;

    case IN_FLASH_AWAY:
      // flash away
      tft_table_cell_value_float(table2, PC_R_D1, PC_C_FAWAY, num / TIME_DIV);
      break;

    case IN_NEXT:
      // next away
      tft_table_cell_value_float(table2, PC_R_D1, PC_C_NAWAY, num / TIME_DIV);
      break;

    default:
#ifdef DEBUG
      Serial.println("tft_stagerun default case");
#endif //DEBUG
      break;
  }
}

void tft_stagerun_clear(const u_int8_t pos) {
#ifdef DEBUG
  Serial.println("Clearing TFT run times");
#endif //

  switch (pos) {
    case IN_INIT:
    case IN_INIT_PAUSE:
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_BEEP, "");
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_FACE, "");
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_AWAY, "");
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_REPEAT, "");

      lv_table_set_cell_value(table2, PC_R_D1, PC_C_FLASH, "");
      lv_table_set_cell_value(table2, PC_R_D1, PC_C_FAWAY, "");
      lv_table_set_cell_value(table2, PC_R_D1, PC_C_NAWAY, "");
      break;

    case IN_BEEP:
      // beep
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_BEEP, "");
      break;

    case IN_FACE:
      // face
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_FACE, "");
      break;

    case IN_AWAY:
      // away
      lv_table_set_cell_value(table1, PC_R_D1, PC_C_AWAY, "");
      break;

    case IN_FLASH:
      // flash
      lv_table_set_cell_value(table2, PC_R_D1, PC_C_FLASH, "");
      break;

    case IN_FLASH_AWAY:
      // flash away
      lv_table_set_cell_value(table2, PC_R_D1, PC_C_FAWAY, "");
      break;

    case IN_NEXT:
      // next away
      lv_table_set_cell_value(table2, PC_R_D1, PC_C_NAWAY, "");
      break;
  }
}

void lcd_stagerun_repeat_top(const uint8_t rep) {
}

void lcd_statusprint(const char ch) {
  // Print up a status character in the top right
}

void lcd_statusclear() {
  // Clear the status character in the top right
}

void lcd_print(const char *str) {
  // Output a string to both LCD and serial
  Serial.print(str);
  // TODO write message to a popup?
  //tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  //tft.setTextSize(2);
  //tft.print(str);
}

void lcd_println(const char *str) {
  // Output a string to both LCD and serial
  // with a newline
  Serial.println(str);
  // TODO write message to a popup?
}

void tft_splash(const char *filename) {
  fs::File bmp;

  bmp=LittleFS.open(filename, FILE_READ);

  if (! bmp) {
    Serial.println("Splash file open failed");
  } else {
#ifdef DEBUG
    Serial.println("Splash file open OK");
#endif //DEBUG

    // Display the bmp file to the screen
    tft_drawBmp(bmp, 0, 0);
    bmp.close();
#ifdef DEBUG
    Serial.println("Splash done");
#endif //DEBUG
  }
}

void tft_drawBmp(fs::File &f, int16_t x, int16_t y) {
  // Hacked from Bodmer's TFT_eSPI utility to take an already opened
  // file and use standard AdaFruit_GFX primatives.

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;
#ifdef DEBUG
  uint32_t startTime = millis();
#endif //DEBUG

  if (read16(f) == 0x4D42) {
    read32(f);
    read32(f);
    seekOffset = read32(f);
    read32(f);
    w = read32(f);
    h = read32(f);

    if ((read16(f) == 1) && (read16(f) == 24) && (read32(f) == 0)) {
      y += h - 1;
      f.seek(seekOffset);
      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        f.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.drawRGBBitmap(x, y--, (uint16_t*)lineBuffer, w, 1);
      }
    }
#ifdef DEBUG
    Serial.print("BMP loaded in "); Serial.print(millis() - startTime);
    Serial.println(" ms");
#endif //DEBUG
  } else {
    Serial.println("BMP format not recognized.");
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
