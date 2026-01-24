#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: monofont_table_style
lv_style_t *get_style_monofont_table_style_MAIN_DEFAULT();
lv_style_t *get_style_monofont_table_style_ITEMS_DEFAULT();
lv_style_t *get_style_monofont_table_style_SCROLLBAR_DEFAULT();
void add_style_monofont_table_style(lv_obj_t *obj);
void remove_style_monofont_table_style(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/