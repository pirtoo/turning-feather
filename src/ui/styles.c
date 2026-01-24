#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: monofont_table_style
//

void init_style_monofont_table_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_border_width(style, 2);
    lv_style_set_border_post(style, false);
    lv_style_set_border_side(style, LV_BORDER_SIDE_NONE);
    lv_style_set_outline_width(style, 2);
    lv_style_set_outline_pad(style, 0);
    lv_style_set_shadow_width(style, 1);
    lv_style_set_shadow_ofs_x(style, 0);
    lv_style_set_shadow_ofs_y(style, 0);
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_decor(style, LV_TEXT_DECOR_NONE);
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
};

lv_style_t *get_style_monofont_table_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_monofont_table_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_monofont_table_style_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_right(style, 2);
    lv_style_set_pad_row(style, 2);
    lv_style_set_pad_column(style, 2);
};

lv_style_t *get_style_monofont_table_style_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_monofont_table_style_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_monofont_table_style_SCROLLBAR_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 1);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_dash_width(style, 0);
    lv_style_set_line_dash_gap(style, 0);
    lv_style_set_arc_width(style, 0);
};

lv_style_t *get_style_monofont_table_style_SCROLLBAR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_monofont_table_style_SCROLLBAR_DEFAULT(style);
    }
    return style;
};

void add_style_monofont_table_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_monofont_table_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_monofont_table_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_monofont_table_style_SCROLLBAR_DEFAULT(), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
};

void remove_style_monofont_table_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_monofont_table_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_monofont_table_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_monofont_table_style_SCROLLBAR_DEFAULT(), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_monofont_table_style,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_monofont_table_style,
    };
    remove_style_funcs[styleIndex](obj);
}

