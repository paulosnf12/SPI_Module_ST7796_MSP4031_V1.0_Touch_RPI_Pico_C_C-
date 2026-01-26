/**
 * @file screen_animations_gen.c
 * @brief Tela principal da aplicação
 *
 * Nesta tela eu concentro os principais elementos da interface:
 *  - Visor principal (medidor)
 *  - Knob infinito para ajuste de valores
 *  - Lista lateral animada (menu rápido)
 *  - Botão Home para retorno ao menu principal
 *
 * Toda a navegação e animações desta tela são controladas aqui.
 *
 * @author João Paulo Magalhães Fernandes
 * @date 2023-06-15
 */

/*********************
 * INCLUDES
 *********************/

/* Header desta própria tela */
#include "screen_animations_gen.h"

/* Animações globais da UI */
#include "../ui_animations.h"

/* Componentes da interface */
#include "../components/visor/visor_gen.h"
#include "../components/arc/infinite_knob.h"
#include "../components/list/list_gen.h"
#include "../components/buttons/home_button.h"

/* Tela de menu principal */
#include "../screens/screen_menu_gen.h"

#include "../components/label/wifi_gen.h"

/*********************
 * ASSETS
 *********************/

/* Logo da HWIT utilizado no canto superior da tela */
LV_IMG_DECLARE(logo_hwit);

/*********************
 * STATIC VARIABLES
 *********************/

/* Cache da tela principal
 * Evita recriação desnecessária da tela */
static lv_obj_t * screen_animations = NULL;

/* Cache da lista lateral (menu deslizante) */
static lv_obj_t * button_list = NULL;

/***********************
 * STATIC PROTOTYPES
 ***********************/

/* Criação da timeline de abertura da tela */
static lv_anim_timeline_t * timeline_screen_open_create(lv_obj_t * obj);

/* Callback para liberar corretamente as timelines */
static void free_timeline_event_cb(lv_event_t * e);

/* Callbacks relacionados ao gesto de swipe */
static void swipe_event_cb(lv_event_t * e);

/* Função auxiliar usada na animação horizontal da lista */
static void anim_list_x(lv_obj_t * obj, int32_t v);

/* Controle de abertura e fechamento da lista lateral */
static void open_list_with_anim(lv_obj_t * list);
static void close_list_with_anim(lv_obj_t * list);
static void hide_list_ready_cb(lv_anim_t * a);

/* Callback do botão Home */
static void home_button_event_cb(lv_event_t * e);

static lv_timer_t * clock_timer = NULL;

/* Callback para atualizar o relógio a cada segundo */
static void clock_timer_cb(lv_timer_t * timer) {
    wifi_label_update();
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Cria a tela principal com todos os seus componentes
 *
 * Caso a tela já exista, eu apenas retorno o ponteiro
 * para evitar recriação e desperdício de memória.
 */
lv_obj_t * screen_animations_create(void)
{
    /* Se a tela já foi criada anteriormente, reutilizo */
    if (screen_animations)
        return screen_animations;

    LV_TRACE_OBJ_CREATE("begin");

    /***************************
     * ROOT SCREEN
     ***************************/
    /* Crio o objeto raiz da tela */
    screen_animations = lv_obj_create(NULL);
    lv_obj_set_name_static(screen_animations, "screen_animations");

    /* Dimensão fixa do display */
    lv_obj_set_size(screen_animations, 480, 320);

    /* Cor de fundo escura para visual profissional */
    lv_obj_set_style_bg_color(screen_animations,
                              lv_color_hex(0x1A1A1A), 0);

    /***************************
     * LISTA LATERAL (TOP LAYER)
     ***************************/
    /* A lista lateral fica no top layer para sobrepor a tela */
    button_list = list_create(lv_layer_top());
    lv_obj_set_name(button_list, "button_list");
    lv_obj_set_size(button_list, 220, 300);

    /* Inicialmente deixo fora da tela, à direita */
    lv_obj_set_pos(button_list, 480, 10);

    /* Começa oculta */
    lv_obj_add_flag(button_list, LV_OBJ_FLAG_HIDDEN);

    /***************************
     * VISOR
     ***************************/
    /* Crio o visor principal (medidor) */
    lv_obj_t * visor = visor_create(screen_animations);

    /* Centralizado no topo da tela */
    lv_obj_set_align(visor, LV_ALIGN_TOP_MID);
    lv_obj_set_pos(visor, 0, 10);
    lv_obj_set_size(visor, 360, 300);

    /***************************
     * INFINITE KNOB
     ***************************/
    /* Knob infinito usado para ajuste de valores */
    lv_obj_t * knob_widget = infinite_knob_create(screen_animations);

    /* Posicionado no canto inferior esquerdo */
    lv_obj_set_align(knob_widget, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_pos(knob_widget, 30, -10);

    /* Associo o knob diretamente ao visor */
    infinite_knob_bind_to_visor();

    /***************************
     * BOTÃO HOME
     ***************************/
    /* Botão Home para retornar ao menu principal */
    lv_obj_t * btn_home = home_button_create(screen_animations, 56);

    /* Posicionado no canto inferior direito */
    lv_obj_align(btn_home, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

    /* Evento de clique do botão Home */
    lv_obj_add_event_cb(btn_home,
                        home_button_event_cb,
                        LV_EVENT_CLICKED,
                        NULL);

    /***************************
     * LOGO HWIT
     ***************************/
    /* Logo fixo no canto superior esquerdo */
    lv_obj_t * img_logo = lv_image_create(screen_animations);
    lv_image_set_src(img_logo, &logo_hwit);

    /* Elemento apenas visual */
    lv_obj_clear_flag(img_logo, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_pad_all(img_logo, 0, 0);
    lv_obj_set_style_border_width(img_logo, 0, 0);

    /* Ajusto a escala do logo */
    lv_image_set_scale(img_logo, 96);

    /* Forço atualização de layout */
    lv_obj_update_layout(img_logo);

    /* Alinhamento absoluto */
    lv_obj_align(img_logo, LV_ALIGN_TOP_LEFT, -20, -25);

    /* Garante que o logo fique em primeiro plano */
    lv_obj_move_foreground(img_logo);


    wifi_label_create(screen_animations);

    /* Cria um timer para atualizar o relógio a cada 1000ms (1 segundo) */
    if (clock_timer == NULL) {
        clock_timer = lv_timer_create(clock_timer_cb, 1000, NULL);
    }

    /***************************
     * TIMELINE
     ***************************/
    /* Aloco o array de timelines desta tela */
    lv_anim_timeline_t ** at_array =
        lv_malloc(sizeof(lv_anim_timeline_t *)
                  * _SCREEN_ANIMATIONS_TIMELINE_CNT);

    /* Timeline de abertura da tela */
    at_array[SCREEN_ANIMATIONS_TIMELINE_SCREEN_OPEN] =
        timeline_screen_open_create(screen_animations);

    /* Armazeno o array como user_data da tela */
    lv_obj_set_user_data(screen_animations, at_array);

    /* Garante liberação correta ao destruir a tela */
    lv_obj_add_event_cb(screen_animations,
                        free_timeline_event_cb,
                        LV_EVENT_DELETE,
                        at_array);

    /***************************
     * GESTURE (SWIPE)
     ***************************/
    /* Habilito detecção de gesto de swipe */
    lv_obj_add_event_cb(screen_animations,
                        swipe_event_cb,
                        LV_EVENT_GESTURE,
                        NULL);

    LV_TRACE_OBJ_CREATE("finished");
    return screen_animations;
}

/**********************
 * TIMELINE
 **********************/

/**
 * @brief Retorna uma timeline específica associada à tela
 */
lv_anim_timeline_t *
screen_animations_get_timeline(lv_obj_t * obj,
                               screen_animations_timeline_t timeline_id)
{
    if (timeline_id >= _SCREEN_ANIMATIONS_TIMELINE_CNT)
        return NULL;

    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

/* Criação da timeline de abertura */
static lv_anim_timeline_t *
timeline_screen_open_create(lv_obj_t * obj)
{
    LV_UNUSED(obj);
    return lv_anim_timeline_create();
}

/* Liberação das timelines e limpeza do cache */
static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array =
        lv_event_get_user_data(e);

    for (uint32_t i = 0;
         i < _SCREEN_ANIMATIONS_TIMELINE_CNT;
         i++)
    {
        lv_anim_timeline_delete(at_array[i]);
    }

    lv_free(at_array);

    /* Limpo os ponteiros globais */
    screen_animations = NULL;
    button_list = NULL;
}

/**********************
 * LISTA ANIMADA
 **********************/

/* Função auxiliar usada pela animação horizontal */
static void anim_list_x(lv_obj_t * obj, int32_t v)
{
    lv_obj_set_x(obj, v);
}

/* Abre a lista lateral com animação */
static void open_list_with_anim(lv_obj_t * list)
{
    if (!list) return;

    lv_obj_clear_flag(list, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a,
        (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240);
    lv_anim_set_path_cb(&a,
        lv_anim_path_ease_out);
    lv_anim_set_values(&a, 480, 260);
    lv_anim_start(&a);
}

/* Callback chamado ao final da animação de fechamento */
static void hide_list_ready_cb(lv_anim_t * a)
{
    lv_obj_t * list =
        lv_anim_get_user_data(a);

    if (list)
        lv_obj_add_flag(list,
                        LV_OBJ_FLAG_HIDDEN);
}

/* Fecha a lista lateral com animação */
static void close_list_with_anim(lv_obj_t * list)
{
    if (!list) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a,
        (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240);
    lv_anim_set_path_cb(&a,
        lv_anim_path_ease_in);
    lv_anim_set_values(&a,
        lv_obj_get_x(list), 480);
    lv_anim_set_ready_cb(&a,
        hide_list_ready_cb);
    lv_anim_set_user_data(&a, list);
    lv_anim_start(&a);
}

/**********************
 * SWIPE
 **********************/

/**
 * @brief Trata o gesto de swipe do usuário
 *
 * Swipe para a esquerda abre a lista lateral.
 * Swipe para a direita fecha a lista.
 */
static void swipe_event_cb(lv_event_t * e)
{
    if (!button_list) return;

    lv_dir_t dir =
        lv_indev_get_gesture_dir(
            lv_indev_get_act());

    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);

    if (dir == LV_DIR_LEFT && p.x >= 240)
    {
        if (lv_obj_has_flag(button_list,
                            LV_OBJ_FLAG_HIDDEN))
            open_list_with_anim(button_list);
    }
    else if (dir == LV_DIR_RIGHT)
    {
        if (!lv_obj_has_flag(button_list,
                              LV_OBJ_FLAG_HIDDEN))
            close_list_with_anim(button_list);
    }
}

/**********************
 * HOME BUTTON
 **********************/

/**
 * @brief Callback do botão Home
 *
 * Retorna para a tela de menu principal
 * utilizando animação de transição.
 */
static void home_button_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    lv_scr_load_anim(screen_menu_create(),
                     LV_SCR_LOAD_ANIM_MOVE_LEFT,
                     200,
                     0,
                     false);
}
