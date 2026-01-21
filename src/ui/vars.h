#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_ZPT_SIGNAL_STRENGTH_INT = 0,
    FLOW_GLOBAL_VARIABLE_START_STOP = 1,
    FLOW_GLOBAL_VARIABLE_FACE_AWAY = 2,
    FLOW_GLOBAL_VARIABLE_PROG_LIST = 3,
    FLOW_GLOBAL_VARIABLE_STAGE_LIST = 4,
    FLOW_GLOBAL_VARIABLE_PROG_FACE_BAR_INT = 5,
    FLOW_GLOBAL_VARIABLE_PROG_STAGE_BAR_INT = 6,
    FLOW_GLOBAL_VARIABLE_START_STOP_BTN_CHECKED_BOOL = 7,
    FLOW_GLOBAL_VARIABLE_FACE_AWAY_BTN_CHECKED_BOOL = 8,
    FLOW_GLOBAL_VARIABLE_PROG_LIST_SEL_INT = 9,
    FLOW_GLOBAL_VARIABLE_STAGE_LIST_SEL_INT = 10
};

// Native global variables

extern int32_t get_var_zpt_signal_strength_int();
extern void set_var_zpt_signal_strength_int(int32_t value);
extern const char *get_var_start_stop();
extern void set_var_start_stop(const char *value);
extern const char *get_var_face_away();
extern void set_var_face_away(const char *value);
extern const char *get_var_prog_list();
extern void set_var_prog_list(const char *value);
extern const char *get_var_stage_list();
extern void set_var_stage_list(const char *value);
extern int32_t get_var_prog_face_bar_int();
extern void set_var_prog_face_bar_int(int32_t value);
extern int32_t get_var_prog_stage_bar_int();
extern void set_var_prog_stage_bar_int(int32_t value);
extern bool get_var_start_stop_btn_checked_bool();
extern void set_var_start_stop_btn_checked_bool(bool value);
extern bool get_var_face_away_btn_checked_bool();
extern void set_var_face_away_btn_checked_bool(bool value);
extern int32_t get_var_prog_list_sel_int();
extern void set_var_prog_list_sel_int(int32_t value);
extern int32_t get_var_stage_list_sel_int();
extern void set_var_stage_list_sel_int(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/