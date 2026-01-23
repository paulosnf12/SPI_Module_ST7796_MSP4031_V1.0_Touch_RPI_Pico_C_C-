/**
 * @file button_menu_gen.c
 * @brief Implementação do botão HOME circular com ícone centralizado
 *
 * Este módulo encapsula a criação de um botão HOME estilizado,
 * utilizado normalmente como botão de retorno à tela principal.
 */

#include "home_button.h"

/**********************
 * DEFINES
 **********************/

/* Cor de fundo do botão HOME */
#define HOME_BTN_BG_COLOR     0x000000

/* Cor do ícone HOME */
#define HOME_BTN_ICON_COLOR   0x1E90FF

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Cria um botão HOME circular
 *
 * Cria um botão do tipo LVGL (`lv_btn`) com:
 *  - formato circular
 *  - cor de fundo sólida
 *  - ícone HOME centralizado
 *  - sem bordas ou sombras
 *
 * @param parent Objeto pai onde o botão será inserido
 * @param size   Diâmetro do botão (largura = altura)
 *
 * @return Ponteiro para o botão LVGL criado
 */
lv_obj_t * home_button_create(lv_obj_t * parent, lv_coord_t size)
{
    /* Criação do botão base */
    lv_obj_t * btn = lv_btn_create(parent);

    /* Define tamanho e formato circular */
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);

    /* Estilo visual do botão */
    lv_obj_set_style_bg_color(btn,
                              lv_color_hex(HOME_BTN_BG_COLOR),
                              0);
    lv_obj_set_style_bg_opa(btn,
                            LV_OPA_COVER,
                            LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);

    /* Remove comportamento de rolagem */
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    /* Criação do ícone HOME usando símbolo nativo do LVGL */
    lv_obj_t * icon = lv_label_create(btn);
    lv_label_set_text(icon, LV_SYMBOL_HOME);

    /* Estilo do ícone */
    lv_obj_set_style_text_color(icon,
                                lv_color_hex(HOME_BTN_ICON_COLOR),
                                0);
    lv_obj_set_style_text_font(icon,
                               &lv_font_montserrat_32,
                               0);

    /* Centraliza o ícone dentro do botão */
    lv_obj_center(icon);

    return btn;
}
