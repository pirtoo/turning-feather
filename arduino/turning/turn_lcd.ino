/*
 * Write to the featherwing touchscreen LCD and get
 * touch info
  */

#include "SPI.h"
#include <Wire.h>  // this is needed even though we aren't using it

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_STMPE610.h>

#include "turn_lcd.h"

Adafruit_ILI9341 tft=Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts=Adafruit_STMPE610(STMPE_CS);

// format line for printing the program name
#define LCD_PROG_FORMAT "%-" STR(TURN_NAME_LENGTH) "." STR(TURN_NAME_LENGTH) "s"

// Active screen area
static const struct rect screen={{TS_MINX, TS_MINY}, {TS_MAXX, TS_MAXY}};

// Used for buffering
char lcd_line[30];
//static const char lcd_prog_format=

// Co-ords for printing timings
static const uint16_t timing_x=45;
static const uint16_t timing_y_1=26;
static const uint16_t timing_y_2=43;
static const uint16_t timing_y_3=62;
static const uint16_t timing_y_4=85;
static const uint16_t timing_y_5=102;
static const uint16_t timing_y_6=120;
static const uint16_t prognum_x=5;
static const uint16_t stagenum_x=279;
static const uint16_t psnum_y=30;
static const uint16_t pslabeltop_y=72;
static const uint16_t pslabelbot_y=176;

// Button rectangles
static const rect p_up={{0, 24}, {43, 130}};
static const rect p_do={{0, 134}, {43, 240}};
static const rect s_up={{274, 24}, {317, 130}};
static const rect s_do={{274, 134}, {317, 240}};

// Start text background
static const rect start={{53, 150}, {262, 190}};

// Used for the input point, keep around
struct point t;
TS_Point p;

// Screen button presses
uint32_t screen_button_time=0;
uint8_t last_screen_button=0;

// Clearing text when screen full.
uint8_t lcd_print_lines=0;

bool between(const uint16_t num, const uint16_t low, const uint16_t high) {
  return (num > low) and (num < high);
}

bool in_rect(const struct point *a, const struct rect *box) {
  return (between(a->x, box->a.x, box->b.x) and between(a->y, box->a.y, box->b.y));
}

void ts_remap(struct point *a) {
  uint16_t temp_y=a->y;
  a->y=map(a->x, TS_MINX, TS_MAXX, tft.height(), 0);
  a->x=map(temp_y, TS_MINY, TS_MAXY, tft.width(), 0);
}

void fill_rect(const struct rect *r, const uint16_t colour) {
  tft.fillRect(r->a.x, r->a.y, r->b.x-r->a.x, r->b.y-r->a.y, colour);
}

void draw_rect(const struct rect *r, const uint16_t colour) {
  tft.drawRect(r->a.x, r->a.y, r->b.x-r->a.x, r->b.y-r->a.y, colour);
}

void text_background(const uint16_t x, const uint16_t y, const uint16_t colour) {
  tft.fillRect(x -3, y -3, 39, 27, colour);
}

uint8_t screen_button_num(const struct point *t) {
  if (in_rect(t, &s_do)) {
    return 3;
  } else if (in_rect(t, &s_up)) {
    return 4;
  } else if (in_rect(t, &p_do)) {
    return 5;
  } else if (in_rect(t, &p_up)) {
    return 6;
  }
  return 0;
}

uint8_t lcd_button() {
  // Loop function to handle screen presses
  if (ts.touched()) {
    while (!ts.bufferEmpty()) {
      // Retrieve a point
      p=ts.getPoint();
      t.x=p.x;
      t.y=p.y;
      if (in_rect(&t, &screen)) {
        ts_remap(&t);
        uint8_t button=screen_button_num(&t);
        if (button == last_screen_button and screen_button_time + SCREEN_DEBOUNCE > millis()) {
          //ts.writeRegister8(STMPE_INT_STA, 0xFF);
          return 0;
        }

        if (button) {
#ifdef DEBUG
          Serial.println(button);
#endif // DEBUG
          screen_button_time=millis();
          last_screen_button=button;
          return button;
        }
      }
    }
  }
  return 0;
}


void lcd_buttons() {
  tft.setTextSize(3);

  fill_rect(&p_up, ILI9341_GREEN);
  draw_rect(&p_up, ILI9341_WHITE);
  fill_rect(&p_do, ILI9341_GREEN);
  draw_rect(&p_do, ILI9341_WHITE);
  text_background(prognum_x, psnum_y, ILI9341_BLACK);
  text_background(prognum_x, pslabeltop_y, ILI9341_BLACK);
  text_background(prognum_x, pslabelbot_y, ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(prognum_x, pslabeltop_y);
  tft.print("P+");
  tft.setCursor(prognum_x, pslabelbot_y);
  tft.print("P-");

  fill_rect(&s_up, ILI9341_RED);
  draw_rect(&s_up, ILI9341_WHITE);
  fill_rect(&s_do, ILI9341_RED);
  draw_rect(&s_do, ILI9341_WHITE);
  text_background(stagenum_x, psnum_y, ILI9341_BLACK);
  text_background(stagenum_x, pslabeltop_y, ILI9341_BLACK);
  text_background(stagenum_x, pslabelbot_y, ILI9341_BLACK);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(stagenum_x, pslabeltop_y);
  tft.print("S+");
  tft.setCursor(stagenum_x, pslabelbot_y);
  tft.print("S-");
}

void lcd_face(const bool isface) {
  // Write to the LCD face/away
  tft.setTextSize(5);
  tft.setCursor(70, 202);
  if (isface) {
    tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
    tft.print(" FACE ");
  } else {
    tft.setTextColor(ILI9341_CYAN, ILI9341_RED);
    tft.print(" AWAY ");
  }
}

void lcd_stop(const bool isstop) {
  // Write to the LCD face/away
  tft.setTextSize(5);
  if (isstop) {
    tft.setTextColor(ILI9341_CYAN, ILI9341_RED);
    tft.setCursor(70, 150);
    tft.print(" STOP ");
  } else {
    // stop is longer than run, clear background
    fill_rect(&start, ILI9341_BLACK);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
    tft.setCursor(85, 150);
    tft.print(" RUN ");
  }
}

void lcd_prog(const char *progname, const uint8_t prognum) {
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(3, 0);
  sprintf(lcd_line, LCD_PROG_FORMAT, progname);
  //sprintf(lcd_line, "%-16.16s", progname);
  tft.print(lcd_line);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(prognum_x, psnum_y);
  // Program num displayed, from 1
  sprintf(lcd_line, "%02d", prognum +1);
  tft.print(lcd_line);
}

void lcd_stage(const struct StageConfig *stage, const uint8_t stagenum) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(stagenum_x, psnum_y);
  tft.setTextSize(3);
  // Stage num displayed, from 1 not 0
  sprintf(lcd_line, "%02d", stagenum +1);
  tft.print(lcd_line);
  
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(timing_x, timing_y_1);
  sprintf(lcd_line, "%4.1f %5.1f %5.1f %2d",
    stage->beep / TIME_DIV,
    stage->face / TIME_DIV,
    stage->away / TIME_DIV,
    stage->repeat);
  tft.print(lcd_line);
  tft.setCursor(timing_x, timing_y_2);
  tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
  tft.print("  B   FACE  AWAY RE");

  tft.setCursor(timing_x, timing_y_4);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  sprintf(lcd_line, "%5.1f %5.1f %5.1f",
    stage->flash / TIME_DIV,
    stage->flashaway / TIME_DIV,
    stage->nextaway / TIME_DIV);
  tft.print(lcd_line);
  tft.setCursor(timing_x, timing_y_5);
  tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
  tft.print("FLASH FAWAY NAWAY");
  tft.setTextColor(ILI9341_BLACK, ILI9341_YELLOW);
  tft.print(" ");
  tft.print(stage->autonext ? "Y" : "N");
}

void lcd_stagerun(const uint16_t num, const uint8_t pos, const uint8_t repeat) {
  // Keep the running numbers up to date
  tft.setTextSize(2);

  switch(pos) {
    case IN_INIT:
      break;

    case IN_INIT_PAUSE:
      break;

    case IN_BEEP:
      // beep
      tft.setCursor(timing_x, timing_y_3);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "%4.1f               ", num / TIME_DIV);
      tft.print(lcd_line);
      lcd_stagerun_clear_bottom();
      break;

    case IN_FACE:
      // face
      tft.setCursor(timing_x, timing_y_3);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "     %5.1f       %2d", num / TIME_DIV, repeat);
      tft.print(lcd_line);
      lcd_stagerun_clear_bottom();
      break;

    case IN_AWAY:
      // away
      tft.setCursor(timing_x, timing_y_3);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "           %5.1f %2d", num / TIME_DIV, repeat);
      tft.print(lcd_line);
      lcd_stagerun_clear_bottom();
      break;

    case IN_FLASH:
      // flash
      lcd_stagerun_clear_top();
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "%5.1f             ", num / TIME_DIV);
      tft.print(lcd_line);
      break;

    case IN_FLASH_AWAY:
      // flash away
      lcd_stagerun_clear_top();
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "      %5.1f       ", num / TIME_DIV);
      tft.print(lcd_line);
      break;

    case IN_NEXT:
      // next away
      lcd_stagerun_repeat_top(repeat);
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      sprintf(lcd_line, "            %5.1f ", num / TIME_DIV);
      tft.print(lcd_line);
      break;

    default:
#ifdef DEBUG
      Serial.println("lcd_stagerun default case");
#endif //DEBUG
      break;
  }
}

void lcd_stagerun_clear() {
#ifdef DEBUG
  Serial.println("Clearing LCD run times");
#endif //
  lcd_stagerun_clear_top();
  lcd_stagerun_clear_bottom();
}

void lcd_stagerun_clear_top() {
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(timing_x, timing_y_3);
  tft.print("                   ");  
}

void lcd_stagerun_repeat_top(const uint8_t rep) {
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(timing_x, timing_y_3);
  sprintf(lcd_line, "                 %2d", rep);
  tft.print(lcd_line);  
}

void lcd_stagerun_clear_bottom() {
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(timing_x, timing_y_6);
  tft.print("                   ");  
}

void lcd_display_set(const struct ProgramConfig *program, uint8_t prognum, uint8_t stagenum) {
  tft.fillScreen(ILI9341_BLACK);
  lcd_buttons();
  lcd_prog(program->longname, prognum);
  lcd_stage(&program->stage[stagenum], stagenum);
}

void lcd_statusprint(const char ch) {
  // Print up a status character in the top right
  tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(299, 0);
  tft.print(ch);
}

void lcd_statusclear() {
  // Clear the status character in the top right
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(299, 0);
  tft.print(' ');
}

void lcd_clear() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  lcd_print_lines=0;
}

void lcd_print(const char *str) {
  // Output a string to both LCD and serial
  Serial.print(str);
  tft.setTextSize(2);
  tft.print(str);
}

void lcd_println(const char *str) {
  // Output a string to both LCD and serial
  // with a newline
  if (lcd_print_lines >= 15) {
    lcd_clear();
  }
  Serial.println(str);
  tft.setTextSize(2);
  tft.println(str);
  lcd_print_lines++;
}

void lcd_setup() {
  tft.begin();
  // Puts the USB port to the lower/right.
  tft.setRotation(3);
  tft.setTextWrap(true);
  tft.fillScreen(ILI9341_BLACK);

  uint8_t i=0;
  while(!ts.begin()) {
    if (i>2) {
      lcd_print("Touchscreen controller");
      lcd_println(" init fail");
    }
    i++;
    delay(1000);
  }

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
}
