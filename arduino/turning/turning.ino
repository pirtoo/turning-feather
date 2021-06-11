/*
 * Turning target controller.
 * Turning feather.
  */

// TODO: Put fudge times into a config file
// TODO: Put debug settings into a config file
// TODO: Put chirp (and chirp time) into config file
// TODO: WIFI/web server
//       - edit device config file
//       - watch current state, provide buttons
//       - ability to edit extra configs
// TODO: add extra turning configs, concat them from
//         main file, write out seperately?
// TODO: Have a directory on SD let you choose config?
//       - or boot with face/away held down to choose
//         files?
// TODO: Text for stage use, eg: "GRCF stage 1"?
//       - will this make the file too large for json?
// TODO: Add SD card insertion detection?
//       - requires CD pin to be soldered on wing
// TODO: Mode to show ZPT signal strength

/*
 * Hardware pins and other definitions
 * Definitions for the Feather HUZZAH32
  */

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

// Unused currently

// Double the value here for battery voltage
//#define BATTERY_VOLTAGE A13

// Unused on the HUZZAH32
#define UNUSED_1 12 // AKA A11, wired to 5v MOSFET
//#define UNUSED_2 A4/36

// Using these to generate rising edge interrupts
// fails badly.
//#define RF_2 36
//#define RF_2 39


// Serial speed
#define SERIAL_SPEED 115200
// Clock rate for timer divider
#define CLOCK_RATE 80

// Extre serial output from the ZPT
#define USE_ZPT_SERIAL

// Number of times to retry the SD card
#define SD_RETRIES 3

// Constants
#define ONE_SECOND 1000000
#define TENTH_SECOND 100000
#define TIME_DIV 10.0

// How long a start beep lasts
#define BEEP_LENGTH ONE_SECOND
// How long to keep the motors going on a turn
#define CHANGE_LENGTH 0.4*ONE_SECOND
// Tenth of a second gives enough repeats to loop properly
#define TURN_RATE TENTH_SECOND

// Keep the face/away lines high while a stage is running
#define KEEP_CHANGE_ACTIVE

// Length of beep chirp when RF stage change happens
// Comment out to disable entirely
#define STAGE_CHANGE_CHIRP 0.4*TENTH_SECOND

// Fudge to allow time for targets to turn
// in 10ths of a second
#define FACE_FUDGE 3
#define AWAY_FUDGE 3

// Number of tenths of seconds (TURN_RATE) from start
// until the targets are faced or the beep, depending
// on config.
#define INIT_PAUSE 6


/*
 * Definitions that need to be early
 */
// Are the targets facing (or in the process of facing)
// vs away or in the process of turning away.
// Are we in a running program or stopped.
bool face=true, turnstop=true;


/*
 * Adafruit featherwing LCD display
 */
#include "turn_lcd.h"
uint8_t lcd_button_num=0;


/*
 * SD card and config file
 */
#include "sdcard.h"


/*
 * Config file
 */
#include "TurnConfig.h"

struct TurnConfig turnconfig;
struct ProgramConfig *currentprog;
struct StageConfig *currentstage;
struct StageConfig runningstage;

const char *turnconf_file="/turnconf.txt";
uint8_t currentstagenum=0, currentprognum=0;


bool checkstagechange(const int change) {
  // Check if a stage change can be made
  return ((currentstagenum + change < currentprog->stages) and
          (currentstagenum + change >= 0));
}

bool changestagenum(const int change, const bool rf_button) {
  // Alter the current stage num within parameters
  if (not checkstagechange(change)) {
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
#endif //STAGE_CHANGE_CHIRP
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


/*
 * Analog multi buttons
 */
#include <AnalogMultiButton.h>

const int buttons_values[2] = {0, 217};
AnalogMultiButton buttons(BUTTONS_PIN, 2, buttons_values);

void buttons_setup() {
  // Make sure all analog systems read the same
  analogReadResolution(10);
}

void buttons_loop() {
  buttons.update();
  if (buttons.onPress(0)) {
#ifdef DEBUG2
    Serial.println("Physical 0 has been pressed");
#endif //DEBUG2
    button_action(2, false);
  }
  if (buttons.onPress(1)) {
#ifdef DEBUG2
    Serial.println("Physical 1 has been pressed");
#endif //DEBUG2
    button_action(1, false);
  }
}


/*
 * RF bravo/ZPT radio control
 */
volatile uint8_t rf_button=0;
portMUX_TYPE rf_buttonmux=portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t rf_buttonsemaphore;

#ifdef USE_ZPT_SERIAL
#include "zpt_serial.h"

void rf_serial_actions() {
  if (zpt_packet_lowbatt(&packetin)) {
    // Print a red on white '!' if remote barttery low
    lcd_statusprint('!');
  } else if (!zpt_packet_learn(&packetin)) {
    // Print a red on white '?' if remote not paired
    lcd_statusprint('?');
  } else {
    // Clear status
    lcd_statusclear();
  }
}
#endif // USE_ZPT_SERIAL

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
#endif // DEBUG2
}
void IRAM_ATTR rfbutton2() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=2;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 2"));
#endif // DEBUG2
}
void IRAM_ATTR rfbutton3() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=3;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 3"));
#endif // DEBUG2
}
void IRAM_ATTR rfbutton4() {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=4;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 4"));
#endif // DEBUG2
}

void button_action(const unsigned int button, const bool rf_button) {
  /*
   * Various button actions to be taken
   * when specific buttons are pressed.
   * Actions are integer numbers.
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
      toggle_face(true);
      break;
    case 3:
      // Action 3, decrement stage number
#ifdef DEBUG
      Serial.println("stage--");
#endif // DEBUG
      if (turnstop) {
        changestagenum(-1, rf_button);
      }
      break;
    case 4:
      // Action 4, increment stage number
#ifdef DEBUG
      Serial.println("stage++");
#endif // DEBUG
      if (turnstop) {
        changestagenum(1, rf_button);
      }
      break;
    case 5:
      // Action 5, decrement program number
#ifdef DEBUG
      Serial.println("program--");
#endif // DEBUG
      if (turnstop) {
        changeprognum(-1);
      }
      break;
    case 6:
      // Action 6, increment program number
#ifdef DEBUG
      Serial.println("program++");
#endif // DEBUG
      if (turnstop) {
        changeprognum(1);
      }
      break;
  }
}


/*
 * Away/face control and timers
 */
hw_timer_t *turntimer=NULL, *changetimer=NULL, *beeptimer=NULL;
volatile SemaphoreHandle_t turnsemaphore;
portMUX_TYPE turnmux=portMUX_INITIALIZER_UNLOCKED;

// Four timers, 0-3, on esp32
#define TURN_TIMER 0
#define CHANGE_TIMER 1
#define BEEP_TIMER 2

// Where in the process are we?
enum stage {
  IN_INIT,
  IN_BEEP,
  IN_FACE,
  IN_AWAY,
  IN_NEXT,
  IN_FLASH,
  IN_FLASH_AWAY,
  IN_INIT_PAUSE
};
enum stage in_stage=IN_INIT;

uint8_t in_repeat=0;
// Are we in the turning fudge time
bool in_fudge=false;

// How many seconds since the turn counter
// started.
volatile uint32_t turncounter=0;

void starttimer(hw_timer_t *timer, const bool resetturn=false) {
  timerStop(timer);
  timerWrite(timer, 0);
  if (resetturn) {
    turncounter=0;
  }
  timerStart(timer);
#ifdef DEBUG_TIMER
  Serial.println("Starting timer");
#endif //DEBUG_TIMER
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
  // Setting digital pins is safe in interrupt handling
  digitalWrite(AWAY_PIN, LOW);
  digitalWrite(FACE_PIN, LOW);
  timerStop(changetimer);
}

void IRAM_ATTR onbeeptimer() {
  // Setting digital pins is safe in interrupt handling
  digitalWrite(BUZZER, LOW);
  timerStop(beeptimer);
}

void start_stage() {
  // Common code used in turntick() to begin a stage
  // without wasting a tick to go to the beep state
  // when beep is zero.

  if (!face) {
    toggle_face(false);
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

void turntick() {
  // Turning target counter has ticked up one count
  portENTER_CRITICAL(&turnmux);
  uint32_t turncount=turncounter;
  portEXIT_CRITICAL(&turnmux);

#ifdef DEBUG2
  Serial.print("Timer: ");
  Serial.println(turncount);
#endif //DEBUG2

  // Where are we in what part of a stage?
  // Main turning targets logic.

  if (turnstop) {
    // We are not running, stop the timer
    timerStop(turntimer);
  } else {
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
          in_stage=IN_BEEP;
          starttimer(turntimer, true);
        } else {
          start_stage();
        }
        break;

      case IN_BEEP:
        // Are we ready to face targets?
        if (currentstage->beep <= turncount) {
          // finished beep pause
          start_stage();
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
            toggle_face(false);
          }
          starttimer(turntimer, true);
          in_stage=IN_FLASH_AWAY;
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
            toggle_face(false);
          }
          in_stage=IN_FACE;
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
            toggle_face(false);
          }
          starttimer(turntimer, true);
          // if we're at the end, what next?
          // do we have an automatic next stage?
          if (currentstage->repeat <= in_repeat) {
            if (currentstage->autonext) {
              // Straight on to the next one, if we can
              in_stage=IN_NEXT;
            } else {
              // We're done
              toggle_stop();
            }
          } else {
            in_stage=IN_AWAY;
          }
        }
        break;

      case IN_AWAY:
        // Targets are in an away time.
        uint8_t c;
        if (currentstage->away <= turncount ) {
          in_fudge=true;
        }
        if (currentstage->away + AWAY_FUDGE <= turncount ) {
          in_fudge=false;
          // finished away pause
          starttimer(turntimer, true);
          if (!face) {
            toggle_face(false);
          }
          in_stage=IN_FACE;
          in_repeat++;
        }
        break;

      case IN_NEXT:
        // Skipping onto the next stage automatically
        // Setting is taken from the previous stage.
        if (not checkstagechange(1)) {
#ifdef DEBUG
          Serial.println("NO AUTONEXT");
#endif //DEBUG
          toggle_stop();
#ifdef DEBUG
        } else {
          Serial.println("AUTONEXT");
#endif //DEBUG
        }
        if (currentstage->nextaway <= turncount) {
          in_fudge=true;
        }
        if (currentstage->nextaway + AWAY_FUDGE <= turncount) {
          in_fudge=false;
          // Start the next program
          changestagenum(1, false);
          prog_init();
          if (currentstage->beep > 0) {
            in_stage=IN_BEEP;
            beep(BEEP_LENGTH);
          } else {
            start_stage();
          }
        }
      break;

    }
  }
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
    onbeeptimer();
    timerStop(turntimer);
    // In case anything is still running,
    // set a timer to stop it.
    timerWrite(changetimer, 0);
    timerStart(changetimer);
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
      toggle_face(false);
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

void toggle_face(bool use_timer) {
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
#ifdef KEEP_CHANGE_ACTIVE
  if (use_timer) {
#endif //KEEP_CHANGE_ACTIVE
    // Start the timer with an alarm to switch
#ifdef DEBUG
    Serial.println("change timer set");
#endif //DEBUG
    timerWrite(changetimer, 0);
    timerStart(changetimer);
#ifdef KEEP_CHANGE_ACTIVE
  }
#endif //KEEP_CHANGE_ACTIVE
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


/*
 * Setup and loop
 */
void setup() {
  // outputs setup
  // Make aure the 12v mosfet drivers are off
  pinMode(FACE_PIN, OUTPUT);
  digitalWrite(FACE_PIN, LOW);
  pinMode(AWAY_PIN, OUTPUT);
  digitalWrite(AWAY_PIN, LOW);
  // Set up other general pins
  pinMode(BUZZER, OUTPUT);
  pinMode(UNUSED_1, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(UNUSED_1, LOW);
  pinMode(BUTTONS_PIN, INPUT);


  // Serial setup
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    ; // wait for serial port
  }
  Serial.println("\r\n");


  // Basic LCD setup
  lcd_setup();
  storage_init();

  lcd_splash();
  Serial.println("\r\n");
  lcd_println("Turning feather controller");
  lcd_println("(c) pir 2019");
  Serial.println("");


  delay(1000);


  // Get the config file from SD or SPIFFS
  File turnfile=turn_file_init();

  // This may fail if the JSON is invalid
  if (!deserializeTurnConfig(turnfile, turnconfig)) {
    lcd_clear();
    lcd_println("Config file not valid");
    while (1);
  }

  char line[4];
  lcd_print("Loaded ");
  sprintf(line, "%d", turnconfig.programs);
  lcd_print(line);
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
  pinMode(RF_1, INPUT);
  pinMode(RF_2, INPUT);
  pinMode(RF_3, INPUT);
  pinMode(RF_4, INPUT);
  attachInterrupt(digitalPinToInterrupt(RF_1), rfbutton1, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_2), rfbutton2, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_3), rfbutton3, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_4), rfbutton4, RISING);
  rf_buttonsemaphore=xSemaphoreCreateBinary();


  // Timer and face/away setup.

  // Semaphores
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

#ifdef DEBUG
  Serial.println(F("\r\nSetup finished.\r\n"));
#endif //DEBUG
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
  // Take screen button press actions
  button_action(lcd_button(), false);

  // Physical buttons action
  buttons_loop();

#ifdef USE_ZPT_SERIAL
  // ZPT serial handling
  zpt_serial_loop();
  if (zpt_serialpacket_ready) {
    rf_serial_actions();
    // Signal ready for another packet.
    zpt_serialpacket_ready=false;
  }
#endif //USE_ZPT_SERIAL

  yield();
}
