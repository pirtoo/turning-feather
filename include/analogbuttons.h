/*
 * Analog multi buttons
 */

#ifndef TF_ANALOGBUTTONS_H
#include <AnalogMultiButton.h>

// Physical buttons pin
#define BUTTONS_PIN 39 // A3

void buttons_setup();
void buttons_loop();

#endif //TF_ANALOGBUTTONS_H