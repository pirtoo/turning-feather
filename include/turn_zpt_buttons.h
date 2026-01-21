/*
 * RF buttons, connected via ZPT module
 */

#ifdef ZPT_BUTTONS
#ifndef TURN_ZPT_BUTTONS_H
#define TURN_ZPT_BUTTONS_H

#define ZPT_1 4
#ifdef FEATHER_ESP32_V2
#define ZPT_2 37
#else
#define ZPT_2 21
#endif //FEATHER_ESP32_V2
#define ZPT_3 27
#define ZPT_4 34

uint8_t get_zpt_button(void);
void zpt_buttons_setup(void);
void zpt_buttons_loop(void);

#endif //TURN_ZPT_BUTTONS_H
#endif //ZPT_BUTTONS
