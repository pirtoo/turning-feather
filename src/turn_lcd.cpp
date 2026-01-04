/*
 * Write to the featherwing touchscreen LCD and get
 * touch info
 */

#include <Arduino.h>
#include "SPI.h"
#include <Wire.h> // this is needed even though we aren't using it

#include "turn_lcd.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSans12pt7b.h>
// This font could be stripped down to only include the letters for
// STOP START FACE AWAY == 10 characters
// to save space.
#include <Fonts/FreeSans24pt7b.h>
// Graphics display driver
Adafruit_ILI9341 tft=Adafruit_ILI9341(TFT_CS, TFT_DC);


// Touchscreen driver changes depending on V1 or V2 of Featherwing display
#ifdef TFT_FW_24_V2
// This is the V2 version of the feartherwing
#include <Adafruit_TSC2007.h>
// If you're using the TSC2007 there is no CS pin needed, so instead its an IRQ
#define TSC_IRQ STMPE_CS
// V2 display touch contoller
Adafruit_TSC2007 ts=Adafruit_TSC2007();
uint16_t z1, z2;
#else
// This is the V1 version of the featherwing
#include <Adafruit_STMPE610.h>
Adafruit_STMPE610 ts=Adafruit_STMPE610(STMPE_CS);
TS_Point p;
#endif //TFT_FW_24_V2


// format line for printing the program name
#define LCD_PROG_FORMAT "%-" STR(TURN_NAME_LENGTH) "." STR(TURN_NAME_LENGTH) "s"

// Active screen area
#ifdef TFT_FW_24_V2
// touch screen coords are reflected in X in the V2
static const struct rect screen={{TS_MAXX, TS_MINY}, {TS_MINX, TS_MAXY}};
#else
static const struct rect screen={{TS_MINX, TS_MINY}, {TS_MAXX, TS_MAXY}};
#endif //TFT_FW_24_V2


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
static const uint16_t pslabeltop_y=71;
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

// Screen button presses
uint32_t screen_button_time=0;
uint8_t last_screen_button=0;

// snprintf buffer
#define BUFF_LEN TURN_NAME_LENGTH+1
char buff[BUFF_LEN];

uint16_t read16(fs::File &f);
uint32_t read32(fs::File &f);

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
  // Handle the correct touchscreen type "button" push

#ifdef TFT_FW_24_V2
  if (! digitalRead(TSC_IRQ)) {
    if (ts.read_touch(&t.x, &t.y, &z1, &z2) && (z1 > 100)) {
#else
  if (ts.touched()) {
    while (!ts.bufferEmpty()) {
      // Retrieve a point
      p=ts.getPoint();
      t.x=p.x;
      t.y=p.y;
#endif //TFT_FW_24_V2
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
  tft.setTextSize(1);
  switch (fsize) {
    case 24:
      tft.setFont(PRETTY_FONT_24);
      break;
    default:
      tft.setFont(PRETTY_FONT_12);
      break;
  }
}

void lcd_defaultfont() {
  tft.setFont();
}

void lcd_drawString(char *text, uint16_t x, uint16_t y) {
  tft.setCursor(x, y);
  tft.print(text);
}

void lcd_buttons() {
  // Draw the buttons and text on them
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
  lcd_drawString("P+", prognum_x +3, pslabeltop_y +18);
  lcd_drawString("P-", prognum_x +3, pslabelbot_y +18);
 
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  lcd_drawString("S+", stagenum_x +3, pslabeltop_y +18);
  lcd_drawString("S-", stagenum_x +3, pslabelbot_y +18);
  lcd_defaultfont();
}

void lcd_face(const bool isface) {
  // Write to the LCD face/away
  lcd_prettyfont(24);
  if (isface) {
    tft.setTextColor(ILI9341_YELLOW);
    fill_rect(&facerect, ILI9341_NAVY);
    lcd_drawString("FACE", 95, 235);
  } else {
    tft.setTextColor(ILI9341_CYAN);
    fill_rect(&facerect, ILI9341_RED);
    lcd_drawString("AWAY", 88, 234);
  }
  lcd_defaultfont();
}

void lcd_stop(const bool isstop) {
  // Write to the LCD start/stop
  lcd_prettyfont(24);
  if (isstop) {
    tft.setTextColor(ILI9341_CYAN);
    fill_rect(&runrect, ILI9341_RED);
    lcd_drawString("STOP", 91, 183);
  } else {
    tft.setTextColor(ILI9341_YELLOW);
    fill_rect(&runrect, ILI9341_NAVY);
    lcd_drawString("RUN", 103, 183);
  }
  lcd_defaultfont();
}

void lcd_prog(const char *progname, const uint8_t prognum) {
  lcd_statusclear();
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  lcd_prettyfont(12);

  // Program name at top of screen
  snprintf(buff, BUFF_LEN, LCD_PROG_FORMAT, progname);
  tft.fillRect(0, 0, tft.width(), 23, ILI9341_BLACK);
  lcd_drawString(buff, 2, 17);

  // Program number
  snprintf(buff, 3, "%02d", prognum +1);
  text_background(prognum_x, psnum_y, ILI9341_BLACK);
  lcd_drawString(buff, prognum_x +4, psnum_y +17);
  lcd_defaultfont();
}

void lcd_stage(const struct StageConfig *stage, const uint8_t stagenum) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  lcd_prettyfont(12);
  sprintf(buff, "%02d", stagenum +1);
  text_background(stagenum_x, psnum_y, ILI9341_BLACK);
  lcd_drawString(buff, stagenum_x +4, psnum_y +17);
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
}

void lcd_print(const char *str) {
  // Output a string to both LCD and serial
  Serial.print(str);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print(str);
}

void lcd_println(const char *str) {
  // Output a string to both LCD and serial
  // with a newline

  Serial.println(str);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println(str);
}

void lcd_splash(const char *filename) {
  fs::File bmp;

//  lcd_clear();
#ifdef LITTLEFS
  bmp = LittleFS.open(filename, FILE_READ);
#else
  bmp = SPIFFS.open(filename, FILE_READ);
#endif //LITTLEFS

  if (! bmp) {
    Serial.println("Splash file open failed");
  } else {
#ifdef DEBUG
    Serial.println("Splash file open OK");
#endif //DEBUG

    // Display the bmp file to the screen and pause
    lcd_drawBmp(bmp, 0, 0);
    bmp.close();
//    stat = reader.drawBMP(filename, tft, 0, 0);
#ifdef DEBUG
    Serial.println("Splash done");
#endif //DEBUG
  }

}

void lcd_setup() {
  delay(10);

#ifdef DEBUG
    Serial.println("Starting touchscreen controller init");
    Serial.println("Trying Featherwing V1 - STMPE610H");
#endif //DEBUG

#ifndef TFT_FW_24_V2
  if (ts.begin()) {
#ifdef DEBUG
    Serial.println("Touchscreen controller STMPE610H OK");
#endif //DEBUG
#else //TFT_FW_24_V2
if (ts.begin(0x48, &Wire)) { 
    pinMode(TSC_IRQ, INPUT);
#ifdef DEBUG
    Serial.println("Touchscreen controller TSC2007 OK");
#endif //DEBUG
#endif //TFT_FW_24_V2
  } else {
    Serial.println("Touchscreen controller init fail");
  }

#ifdef DEBUG
  Serial.println("Start display init");
#endif //DEBUG
  tft.begin();

#ifdef DEBUG
  Serial.println("Display init done");
#endif //DEBUG
  
  // Puts the USB port to the lower/right.
  tft.setRotation(3);
  tft.setTextWrap(true);
  tft.cp437(true);
  tft.fillScreen(ILI9341_BLACK);

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

void lcd_drawBmp(fs::File &f, int16_t x, int16_t y) {
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