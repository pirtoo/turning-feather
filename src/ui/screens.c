#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

static lv_meter_scale_t * scale0;
static lv_meter_indicator_t * indicator1;

static void event_handler_cb_program_prog_dropdown(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_dropdown_get_selected(ta);
            set_var_prog_list_sel_int(value);
        }
    }
}

static void event_handler_cb_program_stage_dropdown(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_dropdown_get_selected(ta);
            set_var_stage_list_sel_int(value);
        }
    }
}

static void event_handler_cb_program_prog_face_bar(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_face_away_btn_checked_bool(value);
        }
    }
}

static void event_handler_cb_program_start_stop_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_start_stop_btn_checked_bool(value);
        }
    }
}

static void event_handler_cb_program_face_away_button(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            set_var_face_away_btn_checked_bool(value);
        }
    }
}

void create_screen_program() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.program = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // prog_dropdown
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.prog_dropdown = obj;
            lv_obj_set_pos(obj, 1, 1);
            lv_obj_set_size(obj, 240, 36);
            lv_dropdown_set_options(obj, "");
            lv_obj_add_event_cb(obj, action_list_select, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_add_event_cb(obj, action_dropdown_checked, LV_EVENT_READY, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_cb_program_prog_dropdown, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ONE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // stage_dropdown
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.stage_dropdown = obj;
            lv_obj_set_pos(obj, 242, 1);
            lv_obj_set_size(obj, 76, 36);
            lv_dropdown_set_options(obj, "");
            lv_obj_add_event_cb(obj, action_list_select, LV_EVENT_VALUE_CHANGED, (void *)2);
            lv_obj_add_event_cb(obj, action_dropdown_checked, LV_EVENT_READY, (void *)2);
            lv_obj_add_event_cb(obj, event_handler_cb_program_stage_dropdown, LV_EVENT_ALL, 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ONE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff282b30), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // prog_face_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.prog_face_bar = obj;
            lv_obj_set_pos(obj, 12, 190);
            lv_obj_set_size(obj, 296, 10);
            lv_obj_add_event_cb(obj, event_handler_cb_program_prog_face_bar, LV_EVENT_ALL, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_INDICATOR | LV_STATE_CHECKED);
        }
        {
            // prog_table
            lv_obj_t *obj = lv_table_create(parent_obj);
            objects.prog_table = obj;
            lv_obj_set_pos(obj, 1, 38);
            lv_obj_set_size(obj, 318, 131);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_ITEMS | LV_STATE_DEFAULT);
        }
        {
            // prog_messagebox
            lv_obj_t *obj = lv_msgbox_create(parent_obj, "", "", 0, true);
            objects.prog_messagebox = obj;
            lv_obj_set_pos(obj, 46, 56);
            lv_obj_set_size(obj, 231, 139);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // start_stop_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.start_stop_button = obj;
            lv_obj_set_pos(obj, 12, 208);
            lv_obj_set_size(obj, 148, 31);
            lv_obj_add_event_cb(obj, action_button_press, LV_EVENT_CLICKED, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_cb_program_start_stop_button, LV_EVENT_ALL, 0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // start_stop_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.start_stop_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // face_away_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.face_away_button = obj;
            lv_obj_set_pos(obj, 174, 208);
            lv_obj_set_size(obj, 134, 31);
            lv_obj_add_event_cb(obj, action_button_press, LV_EVENT_CLICKED, (void *)2);
            lv_obj_add_event_cb(obj, event_handler_cb_program_face_away_button, LV_EVENT_ALL, 0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // face_away_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.face_away_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // prog_stage_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.prog_stage_bar = obj;
            lv_obj_set_pos(obj, 12, 176);
            lv_obj_set_size(obj, 296, 10);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff20ff60), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff20ff60), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_program();
}

void tick_screen_program() {
    {
        const char *new_val = get_var_prog_list();
        const char *cur_val = lv_dropdown_get_options(objects.prog_dropdown);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.prog_dropdown;
            lv_dropdown_set_options(objects.prog_dropdown, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        if (!(lv_obj_get_state(objects.prog_dropdown) & LV_STATE_EDITED)) {
            int32_t new_val = get_var_prog_list_sel_int();
            int32_t cur_val = lv_dropdown_get_selected(objects.prog_dropdown);
            if (new_val != cur_val) {
                tick_value_change_obj = objects.prog_dropdown;
                lv_dropdown_set_selected(objects.prog_dropdown, new_val);
                tick_value_change_obj = NULL;
            }
        }
    }
    {
        const char *new_val = get_var_stage_list();
        const char *cur_val = lv_dropdown_get_options(objects.stage_dropdown);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.stage_dropdown;
            lv_dropdown_set_options(objects.stage_dropdown, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        if (!(lv_obj_get_state(objects.stage_dropdown) & LV_STATE_EDITED)) {
            int32_t new_val = get_var_stage_list_sel_int();
            int32_t cur_val = lv_dropdown_get_selected(objects.stage_dropdown);
            if (new_val != cur_val) {
                tick_value_change_obj = objects.stage_dropdown;
                lv_dropdown_set_selected(objects.stage_dropdown, new_val);
                tick_value_change_obj = NULL;
            }
        }
    }
    {
        bool new_val = get_var_face_away_btn_checked_bool();
        bool cur_val = lv_obj_has_state(objects.prog_face_bar, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.prog_face_bar;
            if (new_val) lv_obj_add_state(objects.prog_face_bar, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.prog_face_bar, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_prog_face_bar_int();
        int32_t cur_val = lv_bar_get_value(objects.prog_face_bar);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.prog_face_bar;
            lv_bar_set_value(objects.prog_face_bar, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = get_var_start_stop_btn_checked_bool();
        bool cur_val = lv_obj_has_state(objects.start_stop_button, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.start_stop_button;
            if (new_val) lv_obj_add_state(objects.start_stop_button, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.start_stop_button, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_start_stop();
        const char *cur_val = lv_label_get_text(objects.start_stop_label);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.start_stop_label;
            lv_label_set_text(objects.start_stop_label, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = get_var_face_away_btn_checked_bool();
        bool cur_val = lv_obj_has_state(objects.face_away_button, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.face_away_button;
            if (new_val) lv_obj_add_state(objects.face_away_button, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.face_away_button, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_face_away();
        const char *cur_val = lv_label_get_text(objects.face_away_label);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.face_away_label;
            lv_label_set_text(objects.face_away_label, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_prog_stage_bar_int();
        int32_t cur_val = lv_bar_get_value(objects.prog_stage_bar);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.prog_stage_bar;
            lv_bar_set_value(objects.prog_stage_bar, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_config_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.config_main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // config_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.config_label = obj;
            lv_obj_set_pos(obj, 67, 9);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Configuration");
        }
        {
            // reset_wifi_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.reset_wifi_button = obj;
            lv_obj_set_pos(obj, 40, 65);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_button_press_config, LV_EVENT_CLICKED, (void *)1);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // reset_wifi_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reset_wifi_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(95), LV_PCT(95));
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Reset WiFi");
                }
            }
        }
        {
            // resert_admin_pw_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.resert_admin_pw_button = obj;
            lv_obj_set_pos(obj, 187, 65);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_button_press_config, LV_EVENT_CLICKED, (void *)2);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // reset_admin_pw_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reset_admin_pw_label = obj;
                    lv_obj_set_pos(obj, 1, 0);
                    lv_obj_set_size(obj, LV_PCT(95), LV_PCT(95));
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Reset Admin");
                }
            }
        }
        {
            // zpt_signal_meter_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.zpt_signal_meter_button = obj;
            lv_obj_set_pos(obj, 40, 150);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_button_press_config, LV_EVENT_CLICKED, (void *)3);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // zpt_signal_meter_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.zpt_signal_meter_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(95), LV_PCT(95));
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ZPT signal");
                }
            }
        }
        {
            // reboot_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.reboot_button = obj;
            lv_obj_set_pos(obj, 187, 150);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_button_press_config, LV_EVENT_CLICKED, (void *)4);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // reboot_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reboot_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(75), LV_PCT(55));
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Reboot");
                }
            }
        }
    }
    
    tick_screen_config_main();
}

void tick_screen_config_main() {
}

void create_screen_zpt_signal() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.zpt_signal = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // zpt_signal_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.zpt_signal_label = obj;
            lv_obj_set_pos(obj, 31, 9);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "ZPT signal strength");
        }
        {
            lv_obj_t *obj = lv_meter_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 14, 46);
            lv_obj_set_size(obj, 180, 180);
            {
                lv_meter_scale_t *scale = lv_meter_add_scale(obj);
                scale0 = scale;
                lv_meter_set_scale_ticks(obj, scale, 41, 1, 5, lv_color_hex(0xffa0a0a0));
                lv_meter_set_scale_major_ticks(obj, scale, 8, 3, 10, lv_color_hex(0xff000000), 10);
                lv_meter_set_scale_range(obj, scale, 0, 160, 300, 120);
                {
                    lv_meter_indicator_t *indicator = lv_meter_add_needle_line(obj, scale, 5, lv_color_hex(0xffffffff), -28);
                    indicator1 = indicator;
                }
            }
        }
        {
            // zpt_signal_back_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.zpt_signal_back_button = obj;
            lv_obj_set_pos(obj, 207, 176);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_button_press_config, LV_EVENT_CLICKED, (void *)5);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // zpt_back_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.zpt_back_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
    }
    
    tick_screen_zpt_signal();
}

void tick_screen_zpt_signal() {
    {
        lv_meter_indicator_t *indicator;
        
        lv_ll_t *indicators = &((lv_meter_t *)objects.obj0)->indicator_ll;
        int index = 0;
        for (indicator = _lv_ll_get_tail(indicators); index > 0 && indicator != NULL; indicator = _lv_ll_get_prev(indicators, indicator), index--);
        
        if (indicator) {
            int32_t new_val = get_var_zpt_signal_strength_int();
            int32_t cur_val = indicator->start_value;
            if (new_val != cur_val) {
                tick_value_change_obj = objects.obj0;
                lv_meter_set_indicator_value(objects.obj0, indicator, new_val);
                tick_value_change_obj = NULL;
            }
        }
    }
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_program,
    tick_screen_config_main,
    tick_screen_zpt_signal,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_program();
    create_screen_config_main();
    create_screen_zpt_signal();
}
