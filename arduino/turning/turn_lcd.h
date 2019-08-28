/****************************************************************/
/* Write to the featherwing touchscreen LCD and get */
/* touch info
/****************************************************************/

#ifndef TURN_LCD
#define TURN_LCD

#include "TurnConfig.h"

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
#ifndef SD_CS
#define SD_CS 14
#endif // SD_CS

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 100
#define TS_MAXX 3800
#define TS_MINY 100
#define TS_MAXY 3750

#define SCREEN_DEBOUNCE 400
//#define SCREEN_DEBOUNCE_SHORT 200

struct point {
  uint16_t x;
  uint16_t y;
};
struct rect {
  point a;
  point b;
};

uint8_t lcd_button();
void lcd_face(const bool isface);
void lcd_stop(const bool isstop);
void lcd_prog(const char *progname, const uint8_t prognum);
void lcd_stage(const StageConfig *stage, const uint8_t stagenum);
void lcd_stagerun(const StageConfig *stage);
void lcd_stagerun_clear();
void lcd_print(const char *str);
void lcd_println(const char *str);
void lcd_display_set(const ProgramConfig *program, uint8_t prognum, uint8_t stagenum);
void lcd_setup();

#endif // TURN_LCD
