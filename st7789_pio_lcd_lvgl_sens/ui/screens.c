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

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // botao_label
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.botao_label = obj;
            lv_obj_set_pos(obj, 195, 251);
            lv_obj_set_size(obj, 90, 35);
        }
        {
            // arc_label
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.arc_label = obj;
            lv_obj_set_pos(obj, 85, 80);
            lv_obj_set_size(obj, 110, 107);
            lv_arc_set_value(obj, 25);
        }
        {
            // chechbox_label
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.chechbox_label = obj;
            lv_obj_set_pos(obj, 329, 80);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Checkbox");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
