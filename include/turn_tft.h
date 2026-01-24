/*
 * Write to the featherwing touchscreen TFT and get
 * touch info
 */

#ifndef TF_TFT_H
#define TF_TFT_H

#include "turning.h"
#include "TurnConfig.h"
#include "sdcard.h"

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>
#include "ui/ui.h"

// Default splash image, bmp, LittleFS
// Convert a bmp to the correct format with:
//   mogrify -format bmp -define bmp:format=bmp4 filename.bmp
#ifndef SPLASH_BMP
#define SPLASH_BMP "/pp1-320x240.bmp"
#endif //SPLASH_BMP

// featherwing pindefs
#ifndef TFT_DC
#define TFT_DC 33
#endif // TFT_DC
#ifndef TFT_CS
#define TFT_CS 15
#endif // TFT_CS
#ifndef STMPE_CS
#define STMPE_CS 32
#endif // STMPE_CS

// This is calibration data for the raw touch
// data to the screen coordinates
#if TSC2007_TS
#define TS_MINX 3800
#define TS_MAXX 300
#define TS_MINY 185
#define TS_MAXY 3700
#else
#define TS_MINX 100
#define TS_MAXX 3800
#define TS_MINY 100
#define TS_MAXY 3750
#endif //TSC2007_TS

// Try to not have calibration data for each screen
#define TOUCH_FUDGE 20

#define SCREEN_DEBOUNCE 400
#define TIME_DIV 10.0

#define PC_R_D1 0
#define PC_R_H1 1
#define PC_R_S1 2

#define PC_C_BEEP 1
#define PC_C_FACE 2
#define PC_C_AWAY 3
#define PC_C_REPEAT 4
#define PC_C_FLASH 1
#define PC_C_FAWAY 2
#define PC_C_NAWAY 3
#define PC_C_AUTON 4

#define PC_CWIDTH 73

struct point {
  uint16_t x;
  uint16_t y;
};
struct rect {
  struct point a;
  struct point b;
};

uint8_t lcd_button();
void lcd_statusprint(const char ch);
void lcd_statusclear();
void lcd_stagerun_repeat_top(const uint8_t rep);
void lcd_print(const char *str);
void lcd_println(const char *str);
void lcd_drawString(char *text, uint16_t x, uint16_t y);

bool ts_read_ready(void);

void setup_tft_screen(void);
void loop_tft_screen(void);

void tft_display_set(const TurnConfig *tc, uint8_t prognum, uint8_t stagenum);
void tft_stop(const bool isstop);
void tft_face(const bool isface, const uint8_t in_stage);
void tft_prog(const uint8_t prognum);
void tft_prog_list(TurnConfig *tc);
void tft_table_cell_value_float(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const float value);
void tft_table_cell_value_int(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const u_int8_t value);
void tft_table_cell_value_bool(lv_obj_t *table, const u_int8_t row, const u_int8_t col, const bool value);
void tft_stage(const StageConfig *stage, const uint8_t stagenum);
void tft_stage_list(const struct StageConfig *stage, const u_int8_t stages, const uint8_t stagenum);
void tft_stagerun(const struct ControlStatus *status, const uint16_t num, const uint8_t pos, const uint8_t repeat);
void tft_stagerun_clear(const u_int8_t pos);

void tft_splash(const char *filename);
void tft_drawBmp(fs::File &f, int16_t x, int16_t y);

#endif //TF_TFT_H
