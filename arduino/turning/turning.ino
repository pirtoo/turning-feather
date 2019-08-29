/*************************************************/
/* Turning target controller. */
/* Turning feather */
/*************************************************/

// TODO: Put fudge times into a config file
// TODO: Put debug settings into a config file
// TODO: Put chirp (and chirp time) into config file
// TODO: yield() ? milticore?
// TODO: WIFI/web server
// TODO: - edit device config file
// TODO: - add extra turning configs, concat them from
//         main file, write out seperately?
// TODO: Have a directory on SD let you choose config?
//       - or boot with face/away held down to choose
//         files?
// TODO: Text for stage use, eg: "GRCF stage 1"?
// TODO: Add SD card insertion detection?
//       - requires CD pin to be soldered on wing
// TODO: Use ZPT serial to show low battery warning
// TODO: Mode to show ZPT signal strength

/*************************************************/
/* Hardware pins and other definitions */
/* Definitions for the Feather HUZZAH32 */
/*************************************************/

// Extra debugging to the serial port while running
//#define DEBUG
//#define DEBUG2

// LED_BUILTIN is 13 on the HUZZAH32
#define BUZZER 13

#define FACE_PIN 25
#define AWAY_PIN 26

// featherwing TFT/TS display
#define TFT_DC 33
#define TFT_CS 15
#define STMPE_CS 32
#define SD_CS 14

// 33 on the logger featherwing
//#define SD_CS SS

#define RF_1 4
#define RF_2 21
#define RF_3 27
#define RF_4 34

// Physical buttons pin
#define BUTTONS_PIN A3 // AKA 39

// Double the value here for battery voltage
//#define BATTERY_VOLTAGE A13

// Unused on the HUZZAH32
//#define UNUSED_1 A4/36
//#define UNUSED_2 A11/12

// Using these to generate rising edge interrupts
// fails badly.
//#define RF_2 36
//#define RF_2 39


// Serial speed
#define SERIAL_SPEED 115200
// Clock rate for timer divider
#define CLOCK_RATE 80

// Number of times to retry the SD card
#define SD_RETRIES 3

// Constants
#define ONE_SECOND 1000000
#define TENTH_SECOND 100000
#define TIME_DIV 10.0

// How long a start beep lasts
#define BEEP_LENGTH ONE_SECOND
// How long to keep the motors going on a turn
#define CHANGE_LENGTH 2*ONE_SECOND
// Tenth of a second gives enough repeats to loop properly
#define TURN_RATE TENTH_SECOND

// Length of beep chirp when RF stage change done
// COmment out to disable entirely
#define STAGE_CHANGE_CHIRP 0.4*TENTH_SECOND

// Fudge to allow time for targets to turn
// in 10ths of a second
#define FACE_FUDGE 3
#define AWAY_FUDGE 3

// Number of tenths of seconds (TURN_RATE) from start
// until the targets are faced or the beep, depending
// on config.
#define INIT_PAUSE 6

// Debounce RF buttons in case of problems
#define RF_BUTTON_DEBOUNCE 200

/*************************************************/
/* Adafruit featherwing LCD display */
/*************************************************/
#include "turn_lcd.h"
uint8_t lcd_button_num=0;
char line[30];


/*************************************************/
/* SD card and config file */
/*************************************************/
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

#include "TurnConfig.h"

TurnConfig turnconfig;
ProgramConfig *currentprog;
StageConfig *currentstage;
StageConfig runningstage;

const char *turnconf_file="/turnconf.txt";
uint8_t currentstagenum=0, currentprognum=0;

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

bool changestagenum(const int change, const bool rf_button) {
  // Alter the current stage num within parameters
  if (currentstagenum + change >= currentprog->stages) {
    return false;
  }
  if (currentstagenum + change < 0) {
    return false;
  }
  currentstagenum=currentstagenum + change;
#ifdef DEBUG
  Serial.print(F("Set stage to "));
  Serial.println(currentstagenum);
#endif //DEBUG
#ifdef STAGE_CHANGE_CHIRP
  if (rf_button) {
    // If changing stages from the remote, chirp
    beep(STAGE_CHANGE_CHIRP);
  }
#endif // STAGE_CHANGE_CHIRP
  updatecurrent();
  lcd_stage(currentstage, currentstagenum);
  return true;
}

bool changeprognum(const int change) {
  // Alter the current stage num within parameters
  if (currentprognum + change >= turnconfig.programs) {
    return false;
  }
  if (currentprognum + change < 0) {
    return false;
  }
  currentprognum=currentprognum + change;
  currentstagenum=0;
#ifdef DEBUG
  Serial.print(F("Set program to "));
  Serial.println(currentprognum);
#endif //DEBUG
  updatecurrent();
  lcd_prog(currentprog->longname, currentprognum);
  lcd_stage(currentstage, currentstagenum);
  return true;
}

void updatecurrent() {
  currentprog=&turnconfig.program[currentprognum];
  currentstage=&currentprog->stage[currentstagenum];  
}


/*************************************************/
/* Analog multi buttons */
/*************************************************/
#include <AnalogMultiButton.h>

const int buttons_values[2] = {0, 217};
AnalogMultiButton buttons(BUTTONS_PIN, 2, buttons_values);

void buttons_setup() {
  // Make sure all analog systems read the same
  analogReadResolution(10);
  // Turn on the pullup
  //pinMode(BUTTONS_PIN, INPUT_PULLUP);
}

void buttons_loop() {
  buttons.update();
  if (buttons.onPress(0)) {
#ifdef DEBUG2
    Serial.println("Physical 0 has been pressed");
#endif //DEBUG
    button_action(2, false);
  }
  if (buttons.onPress(1)) {
#ifdef DEBUG2
    Serial.println("Physical 1 has been pressed");
#endif //DEBUG
    button_action(1, false);
  }
}


/*************************************************/
/* RF bravo/ZPT radio control */
/*************************************************/
volatile uint8_t rf_button=0;
portMUX_TYPE rf_buttonmux=portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t rf_buttonsemaphore;

uint8_t get_rf_button() {
  uint8_t button;
  portENTER_CRITICAL(&rf_buttonmux);
  button=rf_button;
  rf_button=0;
  portEXIT_CRITICAL(&rf_buttonmux);
  return button;
}

void IRAM_ATTR rfbutton1() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=1;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 1"));
#endif // DEBUG
}
void IRAM_ATTR rfbutton2() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=2;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 2"));
#endif // DEBUG
}
void IRAM_ATTR rfbutton3() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=3;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 3"));
#endif // DEBUG
}
void IRAM_ATTR rfbutton4() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=4;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 4"));
#endif // DEBUG
}

void button_action(const unsigned int button, const bool rf_button) {
  /*  Various button actions to be taken
      when specific buttons are pressed.
      Actions are integer numbers.
  */
  switch (button) {
    case 1:
      // Action 1, toggle start/stop
#ifdef DEBUG
      Serial.println("toggle start/stop");
#endif // DEBUG
      toggle_stop();
      break;
    case 2:
      // Action 2, toggle face/away
#ifdef DEBUG
      Serial.println("toggle face/away");
#endif // DEBUG
      toggle_face();
      break;
    case 3:
      // Action 3, decrement stage number
#ifdef DEBUG
      Serial.println("stage--");
#endif // DEBUG
      if (stopped()) {
        changestagenum(-1, rf_button);
      }
      break;
    case 4:
      // Action 4, increment stage number
#ifdef DEBUG
      Serial.println("stage++");
#endif // DEBUG
      if (stopped()) {
        changestagenum(1, rf_button);
      }
      break;
    case 5:
      // Action 3, decrement program number
#ifdef DEBUG
      Serial.println("program--");
#endif // DEBUG
      if (stopped()) {
        changeprognum(-1);
      }
      break;
    case 6:
      // Action 4, increment program number
#ifdef DEBUG
      Serial.println("program++");
#endif // DEBUG
      if (stopped()) {
        changeprognum(1);
      }
      break;
  }
}

/*************************************************/
/* Away/face control and timers */
/*************************************************/
hw_timer_t *turntimer=NULL, *changetimer=NULL, *beeptimer=NULL;
volatile SemaphoreHandle_t turnsemaphore, changesemaphore;
portMUX_TYPE turnmux=portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE changemux=portMUX_INITIALIZER_UNLOCKED;

// Four timers, 0-3, on esp32
#define TURN_TIMER 0
#define CHANGE_TIMER 1
#define BEEP_TIMER 2

// Where in the process are we?
#define IN_INIT 0
#define IN_BEEP 1
#define IN_FACE 2
#define IN_AWAY 3
#define IN_NEXT 4
#define IN_FLASH 5
#define IN_FLASH_AWAY 6
#define IN_INIT_PAUSE 10


uint8_t in_stage=IN_INIT, in_repeat=0;
// Are we in the turning fudge time
bool in_fudge=false;

// How many seconds since the turn counter
// started.
volatile uint32_t turncounter=0;
// Are the targets facing (or in the process of facing)
// vs away or in the process of turning away.
// Are we in a running program or stopped.
bool face=true, turnstop=true;

void starttimer(hw_timer_t *timer, const bool resetturn=false) {
  timerStop(timer);
  timerWrite(timer, 0);
  if (resetturn) {
    turncounter=0;
  }
  timerStart(timer);
#ifdef DEBUG2
  Serial.println("Starting timer");
#endif //DEBUG
}

void IRAM_ATTR onturntimer() {
  // Increment the counter
  portENTER_CRITICAL_ISR(&turnmux);
  turncounter++;
  portEXIT_CRITICAL_ISR(&turnmux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(turnsemaphore, NULL);
}

void IRAM_ATTR onchangetimer() {
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(changesemaphore, NULL);
}

void IRAM_ATTR onbeeptimer() {
  // Setting digital pins is safe in interrupt handling
  digitalWrite(BUZZER, LOW);
  timerStop(beeptimer);
}

void turntick() {
  // Turning target counter has ticked up one count
  portENTER_CRITICAL(&turnmux);
  uint32_t turncount=turncounter;
  portEXIT_CRITICAL(&turnmux);

#ifdef DEBUG2
  Serial.print("Timer: ");
  Serial.println(turncount);
#endif //DEBUG

  // Where are we in what part of a stage?
  // Main turning targets logic.

  if (!turnstop) {
    // We are running
#ifdef DEBUG2
    Serial.print("in_stage: ");
    Serial.println(in_stage);
    Serial.print("in_repeat: ");
    Serial.println(in_repeat);
#endif //DEBUG2

    if (!in_fudge) {
#ifdef DEBUG2
      Serial.println("Updating LCD");
#endif //DEBUG2
      lcd_stagerun(turncount, in_stage, in_repeat);
    }

    switch(in_stage) {
      case IN_INIT_PAUSE:
        // Gap from start if we were facing
        if (INIT_PAUSE > turncount) {
          break;
        }
        // FALLTHROUGH
      case IN_INIT:
        // Initial stage setup/start.
        if (currentstage->beep > 0) {
          beep(BEEP_LENGTH);
          starttimer(turntimer, true);
        }
        in_stage=IN_BEEP;
        break;

      case IN_BEEP:
        // Are we ready to face targets?
        if (currentstage->beep <= turncount) {
          // finished beep pause
          if (!face) {
            toggle_face();
          }
          starttimer(turntimer, true);
          if (currentstage->flash > 0) {
            // Need to do a flash stage.
            in_stage=IN_FLASH;
          } else {
            // No flash, go straight to face
            in_stage=IN_FACE;
          }
          in_repeat=1;
        }
        break;

      case IN_FLASH:
        // Do a single flash exposure.
        if (currentstage->flash <= turncount) {
          in_fudge=true;
        }
        if (currentstage->flash + FACE_FUDGE <= turncount) {
          in_fudge=false;          
          // finished exposure pause
          if (face) {
            toggle_face();
          }
          starttimer(turntimer, true);
          in_stage=IN_FLASH_AWAY;
        }
        break;
  
      case IN_FACE:
        // Targets are in an exposure.
        if (currentstage->face <= turncount) {
          in_fudge=true;
        }
        if (currentstage->face + FACE_FUDGE <= turncount) {
          in_fudge=false;          
          // finished exposure pause
          if (face) {
            toggle_face();
          }
          starttimer(turntimer, true);
          in_stage=IN_AWAY;
        }
        break;
  
      case IN_FLASH_AWAY:
        // Targets are in a flash away time
        if (currentstage->flashaway <= turncount) {
          in_fudge=true;
        }
        if (currentstage->flashaway + AWAY_FUDGE <= turncount) {
          in_fudge=false;          
          // Flash away has finished, off to normal face
          starttimer(turntimer, true);
          if (!face) {
            toggle_face();
          }
          in_stage=IN_FACE;
        }
        break;

      case IN_AWAY:
        // Targets are in an away time.
        // Was that the last repeat?
        if (currentstage->repeat <= in_repeat) {
          // We are done with this stage.
          // Do we auto-increment?
          if (currentstage->autonext) {
            // Straight on to the next one, if we can
            if (changestagenum(1, false)) {
#ifdef DEBUG
              Serial.println("AUTONEXT");
#endif //DEBUG
              in_stage=IN_NEXT;
            }
          } else {
            toggle_stop();
          }
        } else {
          uint8_t c;
          if (currentstage->away <= turncount ) {
            in_fudge=true;
          }
          if (currentstage->away + AWAY_FUDGE <= turncount ) {
            in_fudge=false;
            // finished away pause
            starttimer(turntimer, true);
            if (!face) {
              toggle_face();
            }
            in_stage=IN_FACE;
            in_repeat++;
          }
        }
        break;

      case IN_NEXT:
        // Skipping onto the next stage automatically
        if (currentstage->nextaway <= turncount) {
            in_fudge=true;
        }
        if (currentstage->nextaway + AWAY_FUDGE <= turncount) {
            in_fudge=false;
            // Start the next program
            prog_init();
            in_stage=IN_BEEP;
            if (currentstage->beep > 0) {
              beep(BEEP_LENGTH);
            } 
        }
        break;
    }
  }
}

bool stopped() {
  return turnstop;
}

void prog_init() {
  updatecurrent();
#ifdef DEBUG
  Serial.print("Stage: beep=");
  Serial.print(currentstage->beep);
  Serial.print(" face=");
  Serial.print(currentstage->face);
  Serial.print(" away=");
  Serial.print(currentstage->away);
  Serial.print(" repeat=");
  Serial.print(currentstage->repeat);
  Serial.print(" flash=");
  Serial.print(currentstage->flash);
  Serial.print(" flashaway=");
  Serial.print(currentstage->flashaway);
  Serial.print(" autonext=");
  Serial.print(currentstage->autonext);
  Serial.print(" nextaway=");
  Serial.println(currentstage->nextaway);
#endif //DEBUG
}

void toggle_stop() {
  // Toggle start/stop
  turnstop=!turnstop;
  lcd_stop(turnstop);

  if (turnstop) {
    timerStop(turntimer);
#ifdef DEBUG
    Serial.println("STOP");
#endif //DEBUG

  } else {
#ifdef DEBUG
    Serial.println("START");
#endif //DEBUG
    starttimer(turntimer, true);

    // Make sure targets go away.
    if (face) {
      toggle_face();
      in_stage=IN_INIT_PAUSE;
    } else {
      in_stage=IN_INIT;
    }
    prog_init();
    starttimer(turntimer, true);
  }
  // To stop or start, clear the run times
  lcd_stagerun_clear();
}

void toggle_face() {
  // Toggle face/away
  face=!face;
  timerStop(changetimer);
  lcd_face(face);
  if (face) {
    // Face the target
#ifdef DEBUG
    Serial.println("FACE");  
#endif //DEBUG
    digitalWrite(AWAY_PIN, LOW);
    digitalWrite(FACE_PIN, HIGH);
  } else {
    // Turn the target away
#ifdef DEBUG
    Serial.println("AWAY");
#endif //DEBUG
    digitalWrite(FACE_PIN, LOW);
    digitalWrite(AWAY_PIN, HIGH);
  }
  // Start the timer with an alarm to switch
  timerWrite(changetimer, 0);
  timerStart(changetimer);
}

void finishchange() {
  // pin should only be low for a short time
  timerStop(changetimer);
  digitalWrite(AWAY_PIN, LOW);
  digitalWrite(FACE_PIN, LOW);
#ifdef DEBUG2
  Serial.println("CHANGETIMER END");
#endif //DEBUG
}

void beep(const uint32_t length) {
  digitalWrite(BUZZER, HIGH);
#ifdef DEBUG
  Serial.println("BEEP");
#endif //DEBUG
  timerAlarmWrite(beeptimer, length, true);
  starttimer(beeptimer);
}


/*************************************************/
/* Setup and loop */
/*************************************************/
void setup() {
  // outputs setup
  // Make aure the 12v mosfet drivers are off
  pinMode(FACE_PIN, OUTPUT);
  digitalWrite(FACE_PIN, LOW);
  pinMode(AWAY_PIN, OUTPUT);
  digitalWrite(AWAY_PIN, LOW);

  pinMode(BUZZER, OUTPUT);
    
  // Serial setup
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    ; // wait for serial port
  }
  Serial.println("\r\n");

  // Basic LCD setup
  lcd_setup();
  
  Serial.println("\r\n");
  lcd_println("Turning target controller");
  lcd_println("(c) pir 2019");
  Serial.println("");


  // SD card and main config file setup
  uint8_t count=0;

  while (!SD.begin(SD_CS) and count <= SD_RETRIES) {
    delay(1000);
    // Wait for SD card, loop retrying.
    if (count > 2) {
      lcd_println("SD card:");
      lcd_println(" init failed");
    }
    count++;
  }

  if (!SPIFFS.begin(true)) {
    lcd_println("SPIFFS storage:");
    lcd_println("  init failed");
  }
 
  Serial.print(F("Reading config from: "));
  Serial.println(turnconf_file);
  // Open the file
  File turnfile=checksdfile(turnconf_file);
  if (!turnfile) {
    lcd_println("Can't read config from SD.");
    lcd_println("Trying SPIFFS:");
    turnfile=checkspiffsfile(turnconf_file);
    if (!turnfile) {
      lcd_println("File not found on SPIFFS");
      while (1);
    } else {
      lcd_println("Using backup file from");
      lcd_println("SPIFFS.");
      delay(5000);
    }
  }


  // This may fail if the JSON is invalid
  if (!deserializeTurnConfig(turnfile, turnconfig)) {
    lcd_clear();
    lcd_println("Failed to deserialise");
    lcd_println(" configuration");
    while (1);
  }

  lcd_print("Loaded ");
  sprintf(line, "%10d", turnconfig.programs);
  lcd_print(line);
  //lcd_print(turnconfig.programs);
  lcd_println(" programs");
#ifdef DEBUG
  for (uint8_t i=0; i<turnconfig.programs; i++) {
    Serial.print("  ");
    // Program number displayed from 1 not 0
    Serial.print(i+1);
    Serial.print(" ");
    Serial.println(turnconfig.program[i].longname);
  }
  Serial.println();
#endif

  // Set pointer to initial program and stage
  currentprog=&turnconfig.program[currentprognum];
  currentstage=&currentprog->stage[currentstagenum];


  // Physical buttons setup
  buttons_setup();


  // bravo/ZPT RF setup
  //pinMode(A3, INPUT_PULLUP);
  //pinMode(A4, INPUT_PULLUP);
  pinMode(RF_1, INPUT_PULLUP);
  pinMode(RF_2, INPUT_PULLUP);
  pinMode(RF_3, INPUT_PULLUP);
  pinMode(RF_4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RF_1), rfbutton1, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_2), rfbutton2, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_3), rfbutton3, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_4), rfbutton4, RISING);
  rf_buttonsemaphore=xSemaphoreCreateBinary();

  
  // Timer and face/away setup.
  
  // Semaphores
  changesemaphore=xSemaphoreCreateBinary();
  turnsemaphore=xSemaphoreCreateBinary();

  // Timers, of 4.
  changetimer=timerBegin(CHANGE_TIMER, CLOCK_RATE, true);
  timerStop(changetimer);
  timerWrite(changetimer, 0);
  timerAttachInterrupt(changetimer, &onchangetimer, true);
  timerAlarmWrite(changetimer, CHANGE_LENGTH, true);
  timerAlarmEnable(changetimer);
  
  beeptimer=timerBegin(BEEP_TIMER, CLOCK_RATE, true);
  timerStop(beeptimer);
  timerWrite(beeptimer, 0);
  timerAttachInterrupt(beeptimer, &onbeeptimer, true);
  timerAlarmEnable(beeptimer);

  turntimer=timerBegin(TURN_TIMER, CLOCK_RATE, true);
  timerStop(turntimer);
  timerWrite(turntimer, 0);
  timerAttachInterrupt(turntimer, &onturntimer, true);
  timerAlarmWrite(turntimer, TURN_RATE, true);
  timerAlarmEnable(turntimer);


  // Display setup on the LCD
  lcd_display_set(currentprog, currentprognum, currentstagenum);
  lcd_stop(turnstop);


  Serial.println(F("\r\nSetup finished.\r\n"));
}

void loop() {
  if (xSemaphoreTake(rf_buttonsemaphore, 0) == pdTRUE) {
    // Take RF button press actions
    button_action(get_rf_button(), true);
  }
  if (xSemaphoreTake(turnsemaphore, 0) == pdTRUE) {
    // Take the next action if we are running
    turntick();
  }
  if (xSemaphoreTake(changesemaphore, 0) == pdTRUE) {
    // Turn off facce/away pins afer a time
    finishchange();
  }
  // Take screen button press actions
  button_action(lcd_button(), false);

  // Physical buttons action
  buttons_loop();
  yield();
}
