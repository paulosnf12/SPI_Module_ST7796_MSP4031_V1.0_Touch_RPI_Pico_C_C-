/**
 * @file visor_gen.c
 */

#include "visor_gen.h"
#include "../../ui_animations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**********************
 *   STATIC VARIABLES
 **********************/
static void (*enter_callback)(float) = NULL;
static lv_obj_t * display = NULL;

/**********************
 *   STATIC PROTOTYPES
 **********************/
static void visor_textarea_event_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/******** CREATE ********/
lv_obj_t * visor_create(lv_obj_t * parent)
{
    lv_obj_t * root = lv_obj_create(parent);
    lv_obj_set_name_static(root, "visor");
    lv_obj_set_style_bg_color(root, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_radius(root, 10, 0);
    lv_obj_set_style_border_width(root, 2, 0);
    lv_obj_set_style_border_color(root, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* ---------- DISPLAY DE NÚMEROS ---------- */
    display = lv_textarea_create(root);
    lv_obj_set_name(display, "number_display");
    lv_textarea_set_text(display, "0");
    lv_obj_set_width(display, lv_pct(90));
    lv_obj_set_height(display, lv_pct(80));
    lv_obj_set_style_text_align(display, LV_ARCLABEL_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(display, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(display, &lv_font_montserrat_48, 0);
    lv_obj_set_style_bg_color(display, lv_color_hex(0x001030), 0);
    lv_obj_set_style_border_color(display, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_border_width(display, 2, 0);
    lv_obj_set_style_radius(display, 8, 0);

    /* PERMITIR QUE ENTER SEJA DETECTADO */
    lv_textarea_set_one_line(display, true);
    lv_textarea_set_accepted_chars(display, "0123456789.,+-\n");

    /* EVENTOS */
    lv_obj_add_event_cb(display, visor_textarea_event_cb, LV_EVENT_ALL, NULL);

    return root;
}

/*************************************************
 * Callback do TextArea
 *************************************************/
static void visor_textarea_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    /* ✔ ENTER detectado pelo teclado virtual */
    if (code == LV_EVENT_READY) {
        const char * txt = lv_textarea_get_text(display);
        float value = atof(txt);

        if (enter_callback)
            enter_callback(value);
    }
}

/*************************************************
 * Atualiza o visor
 *************************************************/
void visor_set_value(const char * text)
{
    if (display)
        lv_textarea_set_text(display, text);
}

/*************************************************
 * Registra callback chamado ao apertar ENTER
 *************************************************/
void visor_set_enter_callback(void (*cb)(float))
{
    enter_callback = cb;
}

/*************************************************
 * Retorna o valor atual digitado
 *************************************************/
float visor_get_current_value(void)
{
    if (!display) return 0.0f;

    const char * txt = lv_textarea_get_text(display);
    return atof(txt);
}

void visor_manual_enter(float v)
{
    if (enter_callback)
        enter_callback(v);
}