/*
 * RF buttons, connected via ZPT module
 */

#include "main.h"
#ifdef RF_BUTTONS
#include "turning.h"
#include "turn_rfbuttons.h"

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