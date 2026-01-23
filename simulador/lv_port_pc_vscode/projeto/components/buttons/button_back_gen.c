/**
 * @file button_back_gen.c
 * @brief Componente BUTTON BACK – Botão de retorno de tela
 *
 * Implementa um botão circular de “voltar”, com ícone direcional
 * à esquerda, destinado à navegação entre telas do sistema LVGL.
 * O comportamento (evento) é definido externamente.
 */

#include "button_back_gen.h"

/*********************
 * DEFINES
 *********************/

/* Dimensão padrão do botão (largura = altura) */
#define BTN_BACK_SIZE          48

/* Paleta de cores do botão BACK */
#define BACK_BTN_BG_COLOR      0x000000  /* Fundo */
#define BACK_BTN_ICON_COLOR    0x1E90FF  /* Ícone */
#define BACK_BTN_BORDER_COLOR  0x3A3A3A  /* Borda */

/*********************
 * STATIC FUNCTIONS
 *********************/

/**
 * @brief Aplica o estilo visual padrão ao botão BACK
 *
 * Centraliza toda a definição estética do botão:
 *  - tamanho fixo
 *  - formato circular
 *  - cor de fundo
 *  - borda discreta
 *
 * @param obj Ponteiro para o objeto do botão
 */
static void button_back_apply_style(lv_obj_t * obj)
{
    /* Dimensão e formato */
    lv_obj_set_size(obj, BTN_BACK_SIZE, BTN_BACK_SIZE);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    /* BACKGROUND */
    lv_obj_set_style_bg_color(obj,
                              lv_color_hex(BACK_BTN_BG_COLOR),
                              0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);

    /* BORDER */
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj,
                                  lv_color_hex(BACK_BTN_BORDER_COLOR),
                                  0);
}

/*********************
 * GLOBAL FUNCTIONS
 *********************/

/**
 * @brief Cria um botão BACK com ícone e callback de clique
 *
 * O botão é criado como um container clicável (`lv_obj`)
 * com estilo circular e ícone direcional à esquerda.
 *
 * O callback de evento é opcional e executado no evento
 * `LV_EVENT_CLICKED`.
 *
 * @param parent   Objeto pai onde o botão será criado
 * @param event_cb Callback de clique (pode ser NULL)
 *
 * @return Ponteiro para o objeto do botão criado
 */
lv_obj_t * button_back_create(lv_obj_t * parent, lv_event_cb_t event_cb)
{
    /* Container do botão */
    lv_obj_t * btn = lv_obj_create(parent);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_name(btn, "button_back");

    /* Aplica estilo visual */
    button_back_apply_style(btn);

    /* Registra callback de clique, se fornecido */
    if (event_cb) {
        lv_obj_add_event_cb(btn,
                             event_cb,
                             LV_EVENT_CLICKED,
                             NULL);
    }

    /***************************
     * ÍCONE BACK
     ***************************/

    lv_obj_t * icon = lv_label_create(btn);
    lv_label_set_text(icon, LV_SYMBOL_LEFT);

    lv_obj_set_style_text_font(icon,
                               &lv_font_montserrat_24,
                               0);
    lv_obj_set_style_text_color(icon,
                                lv_color_hex(BACK_BTN_ICON_COLOR),
                                0);

    /* Centraliza o ícone dentro do botão */
    lv_obj_center(icon);

    return btn;
}
