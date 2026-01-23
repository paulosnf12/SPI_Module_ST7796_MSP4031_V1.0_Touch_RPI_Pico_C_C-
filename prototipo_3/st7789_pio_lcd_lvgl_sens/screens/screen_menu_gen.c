/**
 * @file screen_menu_gen.c
 * @brief Tela de menu da aplicação – LVGL
 *
 * Nesta tela eu concentro as opções de menu da aplicação.
 * Ela funciona como uma tela secundária, acessada a partir
 * da tela principal, e permite retorno direto via botão BACK.
 */

/*********************
 * INCLUDES
 *********************/

/* Header desta própria tela */
#include "screen_menu_gen.h"

/* Tela principal (usada para retorno) */
#include "screen_animations_gen.h"

/* Componentes utilizados nesta tela */
#include "../components/menu/time_gen.h"
#include "../components/buttons/button_back_gen.h"

/*********************
 * STATIC VARIABLES
 *********************/

/* Cache da tela de menu
 * Implementado como singleton para evitar múltiplas instâncias */
static lv_obj_t * screen_menu = NULL;

/*********************
 * STATIC FUNCTIONS
 *********************/

/**
 * @brief Callback do botão BACK
 *
 * Este callback é chamado quando o usuário pressiona o botão
 * de voltar. Aqui eu realizo a navegação de retorno para a
 * tela principal, utilizando animação de transição.
 */
static void back_button_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("BACK button clicked");

        /* Retorno para a tela principal.
         * O auto delete é desabilitado para reaproveitar a tela. */
        lv_scr_load_anim(screen_animations_create(),
                         LV_SCR_LOAD_ANIM_MOVE_RIGHT,
                         200,
                         0,
                         false);
    }
}

/*********************
 * GLOBAL FUNCTIONS
 *********************/

/**
 * @brief Cria (ou reutiliza) a tela de menu
 *
 * Esta função cria a tela de menu da aplicação, adicionando
 * seus componentes visuais principais. Caso a tela já exista,
 * eu simplesmente retorno o ponteiro armazenado em cache.
 *
 * @return Ponteiro para o objeto raiz da tela de menu
 */
lv_obj_t * screen_menu_create(void)
{
    /* Se a tela já foi criada anteriormente, reutilizo */
    if (screen_menu)
        return screen_menu;

    LV_TRACE_OBJ_CREATE("begin");

    /***************************
     * ROOT SCREEN
     ***************************/
    /* Crio o objeto raiz da tela de menu */
    screen_menu = lv_obj_create(NULL);
    lv_obj_set_name_static(screen_menu, "screen_menu");

    /* Dimensão fixa do display */
    lv_obj_set_size(screen_menu, 480, 320);

    /* Fundo escuro para manter identidade visual */
    lv_obj_set_style_bg_color(screen_menu,
                              lv_color_hex(0x1A1A1A), 0);

    /***************************
     * TIME CONTAINER (TOP LEFT)
     ***************************/
    /* Widget de tempo posicionado no canto superior esquerdo */
    lv_obj_t * time_widget = time_create(screen_menu);
    lv_obj_align(time_widget, LV_ALIGN_TOP_LEFT, 12, 12);

    /***************************
     * BUTTON BACK (BOTTOM RIGHT)
     ***************************/
    /* Botão de retorno posicionado no canto inferior direito */
    lv_obj_t * btn_back =
        button_back_create(screen_menu, back_button_event_cb);

    lv_obj_align(btn_back,
                 LV_ALIGN_BOTTOM_RIGHT,
                 -12, -12);

    LV_TRACE_OBJ_CREATE("finished");
    return screen_menu;
}
