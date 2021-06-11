/*
 * Write to the featherwing touchscreen LCD and get
 * touch info
  */

#include "SPI.h"
#include <Wire.h>  // this is needed even though we aren't using it

#include "sdcard.h"

#include <TFT_eSPI.h>
TFT_eSPI tft=TFT_eSPI();
// Use these when printing or drawing text in GLCD and high rendering speed fonts
#define GFXFF 1
#define GLCD  0
#define FONT2 2
#define FONT4 4
#define FONT6 6
#define FONT7 7
#define FONT8 8
#define PRETTY_FONT_12 &FreeSans12pt7b
#define PRETTY_FONT_24 &FreeSans24pt7b

// STMPE610 isn't supported by TFT_eSPI
#include <Adafruit_STMPE610.h>
Adafruit_STMPE610 ts=Adafruit_STMPE610(STMPE_CS);

#include "turn_lcd.h"


// format line for printing the program name
#define LCD_PROG_FORMAT "%-" STR(TURN_NAME_LENGTH) "." STR(TURN_NAME_LENGTH) "s"

// Active screen area
static const struct rect screen={{TS_MINX, TS_MINY}, {TS_MAXX, TS_MAXY}};


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

// Run/Stop and Face/away text background
static const rect runrect={{53, 144}, {263, 191}};
static const rect facerect={{53, 197}, {263, 243}};

// Used for the input point, keep around
struct point t;
TS_Point p;

// Screen button presses
uint32_t screen_button_time=0;
uint8_t last_screen_button=0;

// Clearing text when screen full.
uint8_t lcd_print_lines=0;

// sprintf buffer
char buff[TURN_NAME_LENGTH +1];

bool between(const uint16_t num, const uint16_t low, const uint16_t high, const uint8_t fudge) {
  return (num > low - fudge) and (num < high + fudge);
}

bool in_rect(const struct point *a, const struct rect *box, const uint8_t fudge) {
  return (between(a->x, box->a.x, box->b.x, fudge) and
          between(a->y, box->a.y, box->b.y, 0));
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
  if (in_rect(t, &s_do, TOUCH_FUDGE)) {
    return 3;
  } else if (in_rect(t, &s_up, TOUCH_FUDGE)) {
    return 4;
  } else if (in_rect(t, &p_do, TOUCH_FUDGE)) {
    return 5;
  } else if (in_rect(t, &p_up, TOUCH_FUDGE)) {
    return 6;
  }
  return 0;
}

uint8_t lcd_button() {
  // Loop function to handle screen presses
  boolean pressed = ts.touched();
  
  if (ts.touched()) {
    while (!ts.bufferEmpty()) {
      // Retrieve a point
      p=ts.getPoint();
      t.x=p.x;
      t.y=p.y;
      if (in_rect(&t, &screen, 0)) {
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

void lcd_prettyfont(const uint8_t fsize) {
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  switch (fsize) {
    case 24:
      tft.setFreeFont(PRETTY_FONT_24);
      break;
    default:
      tft.setFreeFont(PRETTY_FONT_12);
      break;
  }
}

void lcd_defaultfont() {
  tft.setFreeFont();
}

void lcd_buttons() {
  fill_rect(&p_up, ILI9341_GREEN);
  draw_rect(&p_up, ILI9341_WHITE);
  fill_rect(&p_do, ILI9341_GREEN);
  draw_rect(&p_do, ILI9341_WHITE);
  text_background(prognum_x, psnum_y, ILI9341_BLACK);
  text_background(prognum_x, pslabeltop_y, ILI9341_BLACK);
  text_background(prognum_x, pslabelbot_y, ILI9341_BLACK);

  fill_rect(&s_up, ILI9341_RED);
  draw_rect(&s_up, ILI9341_WHITE);
  fill_rect(&s_do, ILI9341_RED);
  draw_rect(&s_do, ILI9341_WHITE);
  text_background(stagenum_x, psnum_y, ILI9341_BLACK);
  text_background(stagenum_x, pslabeltop_y, ILI9341_BLACK);
  text_background(stagenum_x, pslabelbot_y, ILI9341_BLACK);

  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  lcd_prettyfont(12);
  tft.setTextPadding(25);
  tft.drawString("P+", prognum_x +3, pslabeltop_y +1, GFXFF);
  tft.drawString("P-", prognum_x +3, pslabelbot_y +1, GFXFF);

  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.drawString("S+", stagenum_x +3, pslabeltop_y +1, GFXFF);
  tft.drawString("S-", stagenum_x +3, pslabelbot_y +1, GFXFF);
  lcd_defaultfont();
}

void lcd_face(const bool isface) {
  // Write to the LCD face/away
  lcd_prettyfont(24);
  tft.setTextPadding(0);
  if (isface) {
    tft.setTextColor(ILI9341_YELLOW);
    fill_rect(&facerect, ILI9341_NAVY);
    tft.drawString("FACE", 98, 200, GFXFF);
  } else {
    tft.setTextColor(ILI9341_CYAN);
    fill_rect(&facerect, ILI9341_RED);
    tft.drawString("AWAY", 91, 200, GFXFF);
  }
  lcd_defaultfont();
}

void lcd_stop(const bool isstop) {
  // Write to the LCD face/away
  lcd_prettyfont(24);
  tft.setTextPadding(0);
  if (isstop) {
    tft.setTextColor(ILI9341_CYAN);
    fill_rect(&runrect, ILI9341_RED);
    tft.drawString("STOP", 97, 149, GFXFF);
  } else {
    tft.setTextColor(ILI9341_YELLOW);
    fill_rect(&runrect, ILI9341_NAVY);
    tft.drawString("RUN", 108, 149, GFXFF);
  }
  lcd_defaultfont();
}

void lcd_prog(const char *progname, const uint8_t prognum) {
  lcd_statusclear();
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  lcd_prettyfont(12);
  sprintf(buff, LCD_PROG_FORMAT, progname);
  tft.setTextPadding(tft.width());
  tft.drawString(buff, 3, 0, GFXFF);
  sprintf(buff, "%02d", prognum +1);
  tft.setTextPadding(25);
  tft.drawString(buff, prognum_x +4, psnum_y +1, GFXFF);
  lcd_defaultfont();
}

void lcd_stage(const struct StageConfig *stage, const uint8_t stagenum) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  lcd_prettyfont(12);
  sprintf(buff, "%02d", stagenum +1);
  tft.setTextPadding(25);
  tft.drawString(buff, stagenum_x +4, psnum_y +1, GFXFF);
  lcd_defaultfont();
  
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(timing_x, timing_y_1);
  tft.printf("%4.1f %5.1f %5.1f %2d",
    stage->beep / TIME_DIV,
    stage->face / TIME_DIV,
    stage->away / TIME_DIV,
    stage->repeat);
  tft.setCursor(timing_x, timing_y_2);
  tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
  tft.print("  B   FACE  AWAY RE");

  tft.setCursor(timing_x, timing_y_4);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.printf("%5.1f %5.1f %5.1f",
    stage->flash / TIME_DIV,
    stage->flashaway / TIME_DIV,
    stage->nextaway / TIME_DIV);
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
      tft.printf("%4.1f               ", num / TIME_DIV);
      lcd_stagerun_clear_bottom();
      break;

    case IN_FACE:
      // face
      tft.setCursor(timing_x, timing_y_3);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.printf("     %5.1f       %2d", num / TIME_DIV, repeat);
      lcd_stagerun_clear_bottom();
      break;

    case IN_AWAY:
      // away
      tft.setCursor(timing_x, timing_y_3);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.printf("           %5.1f %2d", num / TIME_DIV, repeat);
      lcd_stagerun_clear_bottom();
      break;

    case IN_FLASH:
      // flash
      lcd_stagerun_clear_top();
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.printf("%5.1f             ", num / TIME_DIV);
      break;

    case IN_FLASH_AWAY:
      // flash away
      lcd_stagerun_clear_top();
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.printf("      %5.1f       ", num / TIME_DIV);
      break;

    case IN_NEXT:
      // next away
      lcd_stagerun_repeat_top(repeat);
      tft.setCursor(timing_x, timing_y_6);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.printf("            %5.1f ", num / TIME_DIV);
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
  tft.printf("                 %2d", rep);
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

void lcd_splash() {
  lcd_clear();
  drawBmp("/pp1-320x240.bmp", 0, 0);
  delay(2000);
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
}


/*
 * Bodmers BMP image rendering function
 * See https://github.com/Bodmer/TFT_eSPI/tree/master/examples/Generic/TFT_SPIFFS_BMP
 */
void drawBmp(const char *filename, int16_t x, int16_t y) {
  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS) {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

#ifdef DEBUG
  uint32_t startTime = millis();
#endif //DEBUG

  if (read16(bmpFS) == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)){
      y += h - 1;

      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
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
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
#ifdef DEBUG
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
#endif //DEBUG
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
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
