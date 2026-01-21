/*
 * Analog multi buttons
 */

#include "analogbuttons.h"
#include "turning.h"

const int buttons_values[2] = {5, 684};
AnalogMultiButton buttons(BUTTONS_PIN, 2, buttons_values);

void buttons_setup() {
  pinMode(BUTTONS_PIN, INPUT);
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