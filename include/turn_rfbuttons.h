/*
 * RF buttons, connected via ZPT module
 */

#ifdef RF_BUTTONS
#ifndef TURN_RFBUTTONS_H
#define TURN_RFBUTTONS_H

#define RF_1 4
#ifdef FEATHER_ESP32_V2
#define RF_2 37
#else
#define RF_2 21
#endif //FEATHER_ESP32_V2
#define RF_3 27
#define RF_4 34

uint8_t get_rf_button(void);
void rf_buttons_setup(void);
void rf_buttons_loop(void);

#endif //TURN_RFBUTTONS_H
#endif //RF_BUTTONS
