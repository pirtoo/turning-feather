#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_button_press(lv_event_t * e);
extern void action_button_press_config(lv_event_t * e);
extern void action_list_select(lv_event_t * e);
extern void action_dropdown_checked(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/