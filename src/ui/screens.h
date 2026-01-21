#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *program;
    lv_obj_t *config_main;
    lv_obj_t *zpt_signal;
    lv_obj_t *prog_face_bar;
    lv_obj_t *start_stop_button;
    lv_obj_t *face_away_button;
    lv_obj_t *prog_dropdown;
    lv_obj_t *stage_dropdown;
    lv_obj_t *prog_table;
    lv_obj_t *prog_messagebox;
    lv_obj_t *start_stop_label;
    lv_obj_t *face_away_label;
    lv_obj_t *prog_stage_bar;
    lv_obj_t *config_label;
    lv_obj_t *reset_wifi_button;
    lv_obj_t *reset_wifi_label;
    lv_obj_t *resert_admin_pw_button;
    lv_obj_t *reset_admin_pw_label;
    lv_obj_t *zpt_signal_meter_button;
    lv_obj_t *zpt_signal_meter_label;
    lv_obj_t *reboot_button;
    lv_obj_t *reboot_label;
    lv_obj_t *zpt_signal_label;
    lv_obj_t *zpt_signal_back_button;
    lv_obj_t *zpt_back_label;
    lv_obj_t *obj0;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_PROGRAM = 1,
    SCREEN_ID_CONFIG_MAIN = 2,
    SCREEN_ID_ZPT_SIGNAL = 3,
};

void create_screen_program();
void tick_screen_program();

void create_screen_config_main();
void tick_screen_config_main();

void create_screen_zpt_signal();
void tick_screen_zpt_signal();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/