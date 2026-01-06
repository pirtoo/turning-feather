/*
 * Turning target controller.
 * Turning feather.
 * 
 * General defines and main functions.
 */

#ifndef TURNING_H
#define TURNING_H

#include <Arduino.h>
#include "main.h"

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

void setup_turnconfig(void);
void updatecurrent(void);
void beep(const uint32_t length);
void button_action(const unsigned int button, const bool rf_button);
void toggle_stop(void);
void toggle_face(bool use_timer);
void prog_init(void);
void turntick(void);
void turntick_loop(void);

void timer_setup(void);
void starttimer(hw_timer_t *timer, const bool resetturn);

uint8_t get_rf_button(void);
void rf_buttons_setup(void);
void rf_buttons_loop(void);

#endif //TURNING_H