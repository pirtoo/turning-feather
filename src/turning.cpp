/*
 * Turning target controller.
 * Turning feather.
 */


// General includes
#include <cstddef>
#include "turning.h"
#include "turn_lcd.h"

#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
#include "turn_neopixel.h"
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL

/*
 * Definitions that need to be early
 */
/*
void IRAM_ATTR rfbutton1(void);
void IRAM_ATTR rfbutton2(void);
void IRAM_ATTR rfbutton3(void);
void IRAM_ATTR rfbutton4(void);
*/
// Are the targets facing (or in the process of facing)
// vs away or in the process of turning away.
// Are we in a running program or stopped.
bool face=true, turnstop=true;

// Config file
#include "TurnConfig.h"
struct TurnConfig turnconfig;
struct ProgramConfig *currentprog;
struct StageConfig *currentstage;
struct StageConfig runningstage;
const char *turnconf_file=TURNCONF;

uint8_t currentstagenum=0, currentprognum=0;

enum stage in_stage=IN_INIT;

uint8_t in_repeat=0;
// Are we in the turning fudge time
bool in_fudge=false;

// How many seconds since the turn counter started.
volatile uint32_t turncounter=0;


void setup_turnconfig() {
  // Get the config file from SD/SPIFFS/LittleFS
  char line[4];
  File turnfile=turn_file_init(turnconf_file);

  // This may fail if the JSON is invalid
  if (!deserializeTurnConfig(turnfile, turnconfig)) {
    lcd_clear();
    lcd_println("Config file not valid");
    while (1) {
      delay(100);
    }
  }

  // Set pointer to initial program and stage
  currentprog=&turnconfig.program[currentprognum];
  currentstage=&currentprog->stage[currentstagenum];

  // Does this get seen on screen?
  // Add a delay after?
  lcd_print("Loaded ");
  snprintf(line, 3, "%d", turnconfig.programs);
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

  // Display setup on the LCD
  lcd_display_set(currentprog, currentprognum, currentstagenum);
  lcd_stop(turnstop);
}

TurnConfig *turnconfig_ptr(void) {
  // Return a pointer to the turnconfig object
  return &turnconfig;
}

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

// TODO Move RF buttons to their own file
#ifdef RF_BUTTONS
/*
 * RF ZPT radio control
 */
volatile uint8_t rf_button=0;
portMUX_TYPE rf_buttonmux=portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t rf_buttonsemaphore;

void rf_buttons_loop(void) {
  if (xSemaphoreTake(rf_buttonsemaphore, 0) == pdTRUE) {
    // Take RF button press actions
    button_action(get_rf_button(), true);
  }
}

uint8_t get_rf_button(void) {
  uint8_t button;
  portENTER_CRITICAL(&rf_buttonmux);
  button=rf_button;
  rf_button=0;
  portEXIT_CRITICAL(&rf_buttonmux);
  return button;
}

void IRAM_ATTR rfbutton1(void) {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=1;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 1"));
#endif // DEBUG2
}
void IRAM_ATTR rfbutton2(void) {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=2;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 2"));
#endif // DEBUG2
}
void IRAM_ATTR rfbutton3(void) {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=3;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 3"));
#endif // DEBUG2
}
void IRAM_ATTR rfbutton4(void) {
  portENTER_CRITICAL(&rf_buttonmux);
  rf_button=4;
  portEXIT_CRITICAL(&rf_buttonmux);
  xSemaphoreGiveFromISR(rf_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 4"));
#endif // DEBUG2
}

void rf_buttons_setup() {
  pinMode(RF_1, INPUT);
  pinMode(RF_2, INPUT);
  pinMode(RF_3, INPUT);
  pinMode(RF_4, INPUT);
  attachInterrupt(digitalPinToInterrupt(RF_1), rfbutton1, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_2), rfbutton2, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_3), rfbutton3, RISING);
  attachInterrupt(digitalPinToInterrupt(RF_4), rfbutton4, RISING);
  rf_buttonsemaphore=xSemaphoreCreateBinary();
}
#endif //RF_BUTTONS

void button_action(const unsigned int button, const bool rf_button) {
  /*
   * Various button actions to be taken when specific buttons are pressed.
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

void starttimer(hw_timer_t *timer, const bool resetturn) {
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

void IRAM_ATTR onturntimer(void) {
  // Increment the counter
  portENTER_CRITICAL_ISR(&turnmux);
  turncounter++;
  portEXIT_CRITICAL_ISR(&turnmux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(turnsemaphore, NULL);
}

void IRAM_ATTR onchangetimer(void) {
  // Setting digital pins is safe in interrupt handling
  digitalWrite(AWAY_PIN, LOW);
  digitalWrite(FACE_PIN, LOW);
  timerStop(changetimer);
}

void IRAM_ATTR onbeeptimer(void) {
  // Setting digital pins is safe in interrupt handling
  digitalWrite(BUZZER, LOW);
  timerStop(beeptimer);
}

void timer_setup() {
  // Semaphores
  turnsemaphore=xSemaphoreCreateBinary();

  // Timers, of 4.
  changetimer=timerBegin(CLOCK_RATE);
  timerStop(changetimer);
  timerWrite(changetimer, 0);
  timerAttachInterrupt(changetimer, &onchangetimer);
  timerAlarm(changetimer, CHANGE_LENGTH, true, 0);
  //timerAlarmEnable(changetimer);

  beeptimer=timerBegin(CLOCK_RATE);
  timerStop(beeptimer);
  timerWrite(beeptimer, 0);
  timerAttachInterrupt(beeptimer, &onbeeptimer);
  //timerAlarmEnable(beeptimer);

  turntimer=timerBegin(CLOCK_RATE);
  timerStop(turntimer);
  timerWrite(turntimer, 0);
  timerAttachInterrupt(turntimer, &onturntimer);
  timerAlarm(turntimer, TURN_RATE, true, 0);
  //timerAlarmEnable(turntimer);
}

void turntick_loop() {
  if (xSemaphoreTake(turnsemaphore, 0) == pdTRUE) {
    // Take the next action if we are running
    turntick();
  }
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
      np(0x00FF00);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
    np(0);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
  
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

    // If using an ESP32 V2 set the neopixel to a different colour
    // depending on which case we are in

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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0xFF0000);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
        break;

      case IN_FLASH:
        // Do a single flash exposure.
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0x00FF00);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0x0000FF);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0x00FF00);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0x0000FF);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
        np(0xFF0000);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
#if defined(HUZZAH32_V2) && defined(ESP_V2_NEOPIXEL)
    np(0);
#endif //HUZZAH32_V2 && ESP_V2_NEOPIXEL
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
  // Work out how to check if the timer is running before stopping it
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

void beep(const uint32_t length) {
  digitalWrite(BUZZER, HIGH);
#ifdef DEBUG
  Serial.println("BEEP");
#endif //DEBUG
  timerAlarm(beeptimer, length, true, 0);
  starttimer(beeptimer, false);
}
