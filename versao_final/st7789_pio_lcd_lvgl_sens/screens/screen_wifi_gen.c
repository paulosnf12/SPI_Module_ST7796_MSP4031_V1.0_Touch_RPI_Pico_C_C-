/**
 * @file screen_wifi_gen.c
 * @brief Tela de configuração Wi-Fi – sincronização de horário
 *
 * Nesta tela eu implemento a interface de configuração de Wi-Fi,
 * permitindo que o usuário informe SSID e senha através de
 * textareas com teclado virtual. Esta tela também é responsável
 * por garantir a limpeza correta dos recursos ao ser fechada.
 */

#include "screen_wifi_gen.h"
#include "screen_menu_gen.h"
#include "../components/buttons/button_back_gen.h"

#include "lvgl.h"
#include "../wifi_handler.h"

#include <string.h>
#include <stdbool.h>

/*********************
 * STATIC VARIABLES
 *********************/

static lv_obj_t * ta_ssid = NULL;
static lv_obj_t * ta_pass = NULL;
static lv_obj_t * keyboard = NULL;

/* Controle de visibilidade da senha */
static bool password_visible = false;

/*********************
 * STATIC FUNCTIONS
 *********************/

static void keyboard_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
    {
        if (keyboard)
        {
            lv_obj_del(keyboard);
            keyboard = NULL;
        }
    }
}

static void textarea_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_FOCUSED)
        return;

    if (keyboard)
        return;

    keyboard = lv_keyboard_create(lv_layer_top());
    lv_obj_set_size(keyboard, 480, 160);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_keyboard_set_textarea(keyboard, lv_event_get_target(e));

    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_ALL, NULL);
}

/* Alterna visibilidade da senha */
static void toggle_pass_visibility_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    password_visible = !password_visible;

    /* Alterna modo senha */
    lv_textarea_set_password_mode(ta_pass, !password_visible);

    /* Atualiza ícone */
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);

    if (password_visible)
        lv_label_set_text(label, LV_SYMBOL_EYE_OPEN);
    else
        lv_label_set_text(label, LV_SYMBOL_EYE_CLOSE);
}

static void connect_button_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    const char * ssid_txt = lv_textarea_get_text(ta_ssid);
    const char * pass_txt = lv_textarea_get_text(ta_pass);

    if (strlen(ssid_txt) == 0) {
        LV_LOG_USER("SSID vazio!");
        return;
    }

    strncpy(wifi_ssid, ssid_txt, sizeof(wifi_ssid) - 1);
    strncpy(wifi_pass, pass_txt, sizeof(wifi_pass) - 1);

    wifi_connect_requested = true;

    if (keyboard)
    {
        lv_obj_del(keyboard);
        keyboard = NULL;
    }
}

static void back_button_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    if (keyboard)
    {
        lv_obj_del(keyboard);
        keyboard = NULL;
    }

    lv_scr_load_anim(screen_menu_create(),
                     LV_SCR_LOAD_ANIM_MOVE_RIGHT,
                     250,
                     0,
                     true);
}

static void screen_delete_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    if (keyboard)
    {
        lv_obj_del(keyboard);
        keyboard = NULL;
    }

    ta_ssid = NULL;
    ta_pass = NULL;
}

/*********************
 * GLOBAL FUNCTIONS
 *********************/

lv_obj_t * screen_wifi_create(void)
{
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, 480, 320);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101820), 0);

    lv_obj_add_event_cb(screen,
                        screen_delete_event_cb,
                        LV_EVENT_DELETE,
                        NULL);

    /* Título */
    lv_obj_t * title = lv_label_create(screen);
    lv_label_set_text(title, "Wi-Fi settings");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 14);

    /* Estilo dos campos */
    static lv_style_t style_ta;
    lv_style_init(&style_ta);
    lv_style_set_bg_color(&style_ta, lv_color_hex(0x1C2633));
    lv_style_set_border_color(&style_ta, lv_color_hex(0x3A4A5A));
    lv_style_set_border_width(&style_ta, 2);
    lv_style_set_radius(&style_ta, 6);
    lv_style_set_text_color(&style_ta, lv_color_hex(0xFFFFFF));
    lv_style_set_pad_left(&style_ta, 10);
    lv_style_set_pad_right(&style_ta, 10);

    /* SSID */
    ta_ssid = lv_textarea_create(screen);
    lv_obj_add_style(ta_ssid, &style_ta, 0);
    lv_obj_set_size(ta_ssid, 360, 44);
    lv_textarea_set_placeholder_text(ta_ssid, "Nome da rede (SSID)");
    lv_obj_align(ta_ssid, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_add_event_cb(ta_ssid, textarea_event_cb, LV_EVENT_FOCUSED, NULL);

    /* SENHA */
    ta_pass = lv_textarea_create(screen);
    lv_obj_add_style(ta_pass, &style_ta, 0);
    lv_obj_set_size(ta_pass, 300, 44);  // menor para caber o botão
    lv_textarea_set_placeholder_text(ta_pass, "Senha");
    lv_textarea_set_password_mode(ta_pass, true);
    lv_obj_align(ta_pass, LV_ALIGN_TOP_MID, -30, 125);
    lv_obj_add_event_cb(ta_pass, textarea_event_cb, LV_EVENT_FOCUSED, NULL);

    /* BOTÃO OLHO */
    lv_obj_t * btn_eye = lv_btn_create(screen);
    lv_obj_set_size(btn_eye, 44, 44);
    lv_obj_align_to(btn_eye, ta_pass, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn_eye, toggle_pass_visibility_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * lbl_eye = lv_label_create(btn_eye);
    lv_label_set_text(lbl_eye, LV_SYMBOL_EYE_CLOSE);
    lv_obj_center(lbl_eye);

    /* BOTÃO CONECTAR */
    lv_obj_t * btn_conn = lv_btn_create(screen);
    lv_obj_set_size(btn_conn, 170, 46);
    lv_obj_align(btn_conn, LV_ALIGN_TOP_MID, 0, 185);
    lv_obj_add_event_cb(btn_conn, connect_button_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * lbl_conn = lv_label_create(btn_conn);
    lv_label_set_text(lbl_conn, "Conectar");
    lv_obj_center(lbl_conn);

    /* BOTÃO VOLTAR */
    lv_obj_t * btn_back = button_back_create(screen, back_button_event_cb);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_RIGHT, -14, -14);

    return screen;
}