/**
 * @file list_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "list_gen.h"
#include "../../ui_animations.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static lv_anim_timeline_t * timeline_list_open_create(lv_obj_t * obj);
static void free_timeline_event_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void generic_button_event_handler(lv_event_t * e)
{
    // Passo A: Obter o botão que foi pressionado
    lv_obj_t * btn = lv_event_get_target(e);
    if (btn == NULL) return;

    // Passo B: Obter a caixa de texto que foi passada como user_data
    lv_obj_t * ta = (lv_obj_t *)lv_event_get_user_data(e);
    if (ta == NULL) return;

    // Passo C: Obter o texto do label do botão
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    if (label == NULL) return;
    const char * txt = lv_label_get_text(label);

    // Passo D: Adicionar o texto à caixa de texto
    lv_textarea_add_text(ta, txt);
}

static void enter_button_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = (lv_obj_t *)lv_event_get_user_data(e);
    if (ta == NULL) return;

    LV_LOG_USER("Enter pressionado! Texto: %s", lv_textarea_get_text(ta));
    // lv_textarea_add_text(ta, "\n"); // Ou qualquer outra lógica
}


lv_obj_t * list_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "list_#");
    lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(lv_obj_0, UNIT_MEDIUM, 0);
    lv_obj_set_style_flex_main_place(lv_obj_0, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(lv_obj_0, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_width(lv_obj_0, lv_pct(100));
    lv_obj_set_height(lv_obj_0, lv_pct(100));
    lv_obj_set_style_radius(lv_obj_0, 10, 0);
    lv_obj_set_style_border_width(lv_obj_0, 2, 0);
    lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_shadow_width(lv_obj_0, 10, 0);
    lv_obj_set_style_shadow_color(lv_obj_0, lv_color_hex(0x102030), 0);

    lv_obj_t * display = lv_textarea_create(lv_obj_0);
    lv_obj_set_name(display, "display");
    lv_obj_set_width(display, lv_pct(95));
    lv_obj_set_height(display, lv_pct(15));
    lv_obj_set_style_text_align(display, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(display, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_color(display, lv_color_hex(0x001030), 0);
    lv_obj_set_style_border_color(display, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_border_width(display, 2, 0);
    lv_obj_set_style_radius(display, 8, 0);
    lv_obj_set_style_pad_all(display, UNIT_SMALL, 0);
    
    lv_obj_t * btn_0 = button_normal_create(lv_obj_0, "0");
    lv_obj_set_name(btn_0, "btn_0");
    lv_obj_set_width(btn_0, lv_pct(30));
    lv_obj_set_style_bg_color(btn_0, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_0, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_0, lv_color_hex(0x2a2a3a), 0);
    lv_obj_set_style_border_color(btn_0, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_1 = button_normal_create(lv_obj_0, "1");
    lv_obj_set_name(btn_1, "btn_1");
    lv_obj_set_width(btn_1, lv_pct(30));
    lv_obj_set_style_bg_color(btn_1, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_1, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_1, lv_color_hex(0x2a2a3a), 0);
    lv_obj_set_style_border_color(btn_1, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_2 = button_normal_create(lv_obj_0, "2");
    lv_obj_set_name(btn_2, "btn_2");
    lv_obj_set_width(btn_2, lv_pct(30));
    lv_obj_set_style_bg_color(btn_2, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_2, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_2, lv_color_hex(0x2a2a3a), 0);
    lv_obj_set_style_border_color(btn_2, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_3 = button_normal_create(lv_obj_0, "3");
    lv_obj_set_name(btn_3, "btn_3");
    lv_obj_set_width(btn_3, lv_pct(30));
    lv_obj_set_style_bg_color(btn_3, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_3, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_3, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_4 = button_normal_create(lv_obj_0, "4");
    lv_obj_set_name(btn_4, "btn_4");
    lv_obj_set_width(btn_4, lv_pct(30));
    lv_obj_set_style_bg_color(btn_4, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_4, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_4, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_5 = button_normal_create(lv_obj_0, "5");
    lv_obj_set_name(btn_5, "btn_5");
    lv_obj_set_width(btn_5, lv_pct(30));
    lv_obj_set_style_bg_color(btn_5, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_5, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_5, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_6 = button_normal_create(lv_obj_0, "6");
    lv_obj_set_name(btn_6, "btn_6");
    lv_obj_set_width(btn_6, lv_pct(30));
    lv_obj_set_style_bg_color(btn_6, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_6, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_6, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_7 = button_normal_create(lv_obj_0, "7");
    lv_obj_set_name(btn_7, "btn_7");
    lv_obj_set_width(btn_7, lv_pct(30));
    lv_obj_set_style_bg_color(btn_7, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_7, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_7, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_8 = button_normal_create(lv_obj_0, "8");
    lv_obj_set_name(btn_8, "btn_8");
    lv_obj_set_width(btn_8, lv_pct(30));
    lv_obj_set_style_bg_color(btn_8, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_8, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_8, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_w = button_normal_create(lv_obj_0, "W");
    lv_obj_set_name(btn_w, "btn_w");
    lv_obj_set_width(btn_w, lv_pct(30));
    lv_obj_set_style_bg_color(btn_w, lv_color_hex(0x0a84ff), 0);
    lv_obj_set_style_text_color(btn_w, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_w, lv_color_hex(0xffffff), 0);
    
    lv_obj_t * btn_9 = button_normal_create(lv_obj_0, "9");
    lv_obj_set_name(btn_9, "btn_9");
    lv_obj_set_width(btn_9, lv_pct(30));
    lv_obj_set_style_bg_color(btn_9, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_9, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_9, lv_color_hex(0x1e90ff), 0);
    
    lv_obj_t * btn_dbm = button_normal_create(lv_obj_0, "dBm");
    lv_obj_set_name(btn_dbm, "btn_dbm");
    lv_obj_set_width(btn_dbm, lv_pct(30));
    lv_obj_set_style_bg_color(btn_dbm, lv_color_hex(0x0a84ff), 0);
    lv_obj_set_style_text_color(btn_dbm, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_dbm, lv_color_hex(0xffffff), 0);
    
    lv_obj_t * btn_enter = button_normal_create(lv_obj_0, "ENTER");
    lv_obj_set_name(btn_enter, "btn_enter");
    lv_obj_set_width(btn_enter, lv_pct(65));
    lv_obj_set_style_bg_color(btn_enter, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_text_color(btn_enter, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(btn_enter, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_enter, lv_color_hex(0x62a8ff), 0);
    lv_obj_set_style_radius(btn_enter, 8, 0);
    
    lv_obj_t * btn_point = button_normal_create(lv_obj_0, ".");
    lv_obj_set_name(btn_point, "btn_point");
    lv_obj_set_width(btn_point, lv_pct(30));
    lv_obj_set_style_bg_color(btn_point, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_color(btn_point, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(btn_point, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_radius(btn_point, 8, 0);

    lv_obj_add_event_cb(btn_0, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_1, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_2, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_3, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_4, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_5, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_6, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_7, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_8, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_9, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_w, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_dbm, generic_button_event_handler, LV_EVENT_CLICKED, display);
    lv_obj_add_event_cb(btn_point, generic_button_event_handler, LV_EVENT_CLICKED, display);

    // Associe um callback diferente para o botão Enter
    lv_obj_add_event_cb(btn_enter, enter_button_event_handler, LV_EVENT_CLICKED, display);
    
    
    /* create animation timeline(s) */
    lv_anim_timeline_t ** at_array = lv_malloc(sizeof(lv_anim_timeline_t *) * _LIST_TIMELINE_CNT);
    at_array[LIST_TIMELINE_LIST_OPEN] = timeline_list_open_create(lv_obj_0);
    lv_obj_set_user_data(lv_obj_0, at_array);
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

lv_anim_timeline_t * list_get_timeline(lv_obj_t * obj, list_timeline_t timeline_id)
{
    if (timeline_id >= _LIST_TIMELINE_CNT) {
        LV_LOG_WARN("list has no timeline with %d ID", timeline_id);
        return NULL;
    }

    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Helper function to execute animations */
static void int_anim_exec_cb(lv_anim_t * a, int32_t v)
{
    uint32_t data = (lv_uintptr_t)lv_anim_get_user_data(a);
    lv_style_prop_t prop = data >> 24;
    lv_style_selector_t selector = data & 0x00ffffff;

    lv_style_value_t style_value;
    style_value.num = v;
    lv_obj_set_local_style_prop(a->var, prop, style_value, selector);
}

static lv_anim_timeline_t * timeline_list_open_create(lv_obj_t * obj)
{
    lv_anim_timeline_t * at = lv_anim_timeline_create();
    lv_anim_timeline_t * at_to_merge = NULL;

    lv_anim_t a;
    uint32_t selector_and_prop;

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, 300);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, 0, &a);
    lv_anim_set_early_apply(&a, true);

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 60, 0);
    lv_anim_set_duration(&a, 300);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, 0, &a);
    lv_anim_set_early_apply(&a, true);

    return at;
}

static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);
    uint32_t i;
    for(i = 0; i < _LIST_TIMELINE_CNT; i++) {
        lv_anim_timeline_delete(at_array[i]);
    }
    lv_free(at_array);
}

