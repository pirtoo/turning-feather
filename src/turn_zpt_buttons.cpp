/*
 * RF buttons, connected via ZPT module
 */

#include "main.h"
#ifdef ZPT_BUTTONS
#include "turning.h"
#include "turn_zpt_buttons.h"

volatile uint8_t zpt_button=0;
portMUX_TYPE zpt_buttonmux=portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t zpt_buttonsemaphore;

void zpt_buttons_loop(void) {
  if (xSemaphoreTake(zpt_buttonsemaphore, 0) == pdTRUE) {
    // Take RF button press actions
    button_action(get_zpt_button(), true);
  }
}

uint8_t get_zpt_button(void) {
  uint8_t button;
  portENTER_CRITICAL(&zpt_buttonmux);
  button=zpt_button;
  zpt_button=0;
  portEXIT_CRITICAL(&zpt_buttonmux);
  return button;
}

void IRAM_ATTR zpt_button1(void) {
  portENTER_CRITICAL(&zpt_buttonmux);
  zpt_button=1;
  portEXIT_CRITICAL(&zpt_buttonmux);
  xSemaphoreGiveFromISR(zpt_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 1"));
#endif // DEBUG2
}
void IRAM_ATTR zpt_button2(void) {
  portENTER_CRITICAL(&zpt_buttonmux);
  zpt_button=2;
  portEXIT_CRITICAL(&zpt_buttonmux);
  xSemaphoreGiveFromISR(zpt_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 2"));
#endif // DEBUG2
}
void IRAM_ATTR zpt_button3(void) {
  portENTER_CRITICAL(&zpt_buttonmux);
  zpt_button=3;
  portEXIT_CRITICAL(&zpt_buttonmux);
  xSemaphoreGiveFromISR(zpt_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 3"));
#endif // DEBUG2
}
void IRAM_ATTR zpt_button4(void) {
  portENTER_CRITICAL(&zpt_buttonmux);
  zpt_button=4;
  portEXIT_CRITICAL(&zpt_buttonmux);
  xSemaphoreGiveFromISR(zpt_buttonsemaphore, NULL);
#ifdef DEBUG2
  Serial.println(F("RF button 4"));
#endif // DEBUG2
}

void zpt_buttons_setup() {
  pinMode(ZPT_1, INPUT);
  pinMode(ZPT_2, INPUT);
  pinMode(ZPT_3, INPUT);
  pinMode(ZPT_4, INPUT);
  attachInterrupt(digitalPinToInterrupt(ZPT_1), zpt_button1, RISING);
  attachInterrupt(digitalPinToInterrupt(ZPT_2), zpt_button2, RISING);
  attachInterrupt(digitalPinToInterrupt(ZPT_3), zpt_button3, RISING);
  attachInterrupt(digitalPinToInterrupt(ZPT_4), zpt_button4, RISING);
  zpt_buttonsemaphore=xSemaphoreCreateBinary();
}
#endif //zpt_buttonS