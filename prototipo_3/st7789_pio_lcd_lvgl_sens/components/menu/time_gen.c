/**
 * @file time_gen.c
 * @brief Widget de configuração de horário
 *
 * Este módulo cria um botão no menu principal que,
 * ao ser clicado, abre a tela de configuração de Wi-Fi
 * e ajuste automático/manual de horário.
 */

#include "time_gen.h"

/* Tela que será aberta ao clicar no botão */
#include "../../screens/screen_wifi_gen.h"

/* Biblioteca padrão de tempo (uso futuro: RTC / sincronização) */
#include <time.h>

/*********************
 * DEFINES
 *********************/

/* Dimensões fixas do botão no menu */
#define TIME_W   96
#define TIME_H   96

/* Cores do botão (tema visual do menu) */
#define HOME_BTN_BG_COLOR     0x000000   /* Fundo preto */
#define HOME_BTN_ICON_COLOR   0x1E90FF   /* Azul (DodgerBlue) */

/*********************
 * STATIC VARIABLES
 *********************/

/*
 * Variáveis estáticas para armazenar o horário definido manualmente.
 * São mantidas neste módulo para evitar acesso externo direto.
 */
static int manual_hour   = 12;
static int manual_minute = 0;

/*********************
 * STATIC FUNCTIONS
 *********************/

/*
 * Callback interno chamado quando o botão é clicado.
 * Não é visível fora deste arquivo.
 */
static void time_click_event_cb(lv_event_t * e);

/*********************
 * GLOBAL FUNCTIONS
 *********************/

/**
 * @brief Define manualmente o horário
 *
 * Esta função é chamada pela tela de ajuste de horário.
 * Os valores são validados antes de serem armazenados.
 *
 * @param hour   Hora (0–23)
 * @param minute Minuto (0–59)
 */
void time_set_manual(int hour, int minute)
{
    /* Validação de faixa */
    if (hour < 0 || hour > 23)     return;
    if (minute < 0 || minute > 59) return;

    /* Atualiza os valores internos */
    manual_hour   = hour;
    manual_minute = minute;

    /*
     * FUTURAS EXTENSÕES:
     * - Gravar horário em RTC externo/interno
     * - Sincronizar com time() ou NTP
     * - Atualizar labels do menu principal
     */
}

/**
 * @brief Cria o botão de horário no menu principal
 *
 * @param parent Objeto pai (geralmente o container do menu)
 * @return Ponteiro para o objeto criado
 */
lv_obj_t * time_create(lv_obj_t * parent)
{
    /* ================= CONTAINER ================= */

    /*
     * Cria um objeto base (container) que funciona como botão
     */
    lv_obj_t * cont = lv_obj_create(parent);

    /* Define tamanho fixo */
    lv_obj_set_size(cont, TIME_W, TIME_H);

    /* Remove scroll (não é uma lista) */
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* Habilita clique */
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);

    /* Estilo visual do botão */
    lv_obj_set_style_radius(cont, 14, 0);                 /* Bordas arredondadas */
    lv_obj_set_style_bg_color(cont,
                              lv_color_hex(HOME_BTN_BG_COLOR),
                              0);                          /* Cor de fundo */
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);        /* Opacidade total */
    lv_obj_set_style_border_width(cont, 0, 0);             /* Sem borda */

    /* ================= ÍCONE ================= */

    /*
     * Ícone central do botão (Wi-Fi / horário automático)
     */
    lv_obj_t * icon = lv_label_create(cont);

    /* Define o símbolo (ícone LVGL) */
    lv_label_set_text(icon, LV_SYMBOL_WIFI);

    /* Fonte grande para ícone */
    lv_obj_set_style_text_font(icon,
                               &lv_font_montserrat_36,
                               0);

    /* Cor do ícone */
    lv_obj_set_style_text_color(icon,
                                lv_color_hex(HOME_BTN_ICON_COLOR),
                                0);

    /* Centraliza o ícone dentro do botão */
    lv_obj_center(icon);

    /* ================= EVENTO ================= */

    /*
     * Associa o evento de clique ao container
     */
    lv_obj_add_event_cb(cont,
                        time_click_event_cb,
                        LV_EVENT_CLICKED,
                        NULL);

    return cont;
}

/*********************
 * STATIC FUNCTIONS
 *********************/

/**
 * @brief Callback de clique do botão de horário
 *
 * Ao clicar:
 *  - Cria a tela de Wi-Fi / horário
 *  - Faz a transição animada para a nova tela
 */
static void time_click_event_cb(lv_event_t * e)
{
    /* Evita warning caso o parâmetro não seja usado */
    LV_UNUSED(e);

    /* Cria a tela de configuração */
    lv_obj_t * wifi_screen = screen_wifi_create();

    /*
     * Carrega a nova tela com animação:
     * - Movimento da direita para a esquerda
     * - Duração: 300 ms
     */
    lv_scr_load_anim(wifi_screen,
                     LV_SCR_LOAD_ANIM_MOVE_LEFT,
                     300,
                     0,
                     false);
}
