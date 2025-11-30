/**
 * @file screen_animations_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "screen_animations_gen.h"
#include "../ui_animations.h"
#include "../components/arc/arc_gen.h"
#include "../components/visor/visor_gen.h"

/***********************
 *  STATIC PROTOTYPES
 ***********************/
static lv_anim_timeline_t * timeline_screen_open_create(lv_obj_t * obj);
static void free_timeline_event_cb(lv_event_t * e);
static void botao_seta_toggle_cb(lv_event_t * e);

/* Prototypes novos */
static void swipe_event_cb(lv_event_t * e);
static void anim_list_x(lv_obj_t * obj, int32_t v);
static void open_list_with_anim(lv_obj_t * list);
static void close_list_with_anim(lv_obj_t * list);

/* NOVO: callback para esconder após animação terminar */
static void hide_list_ready_cb(lv_anim_t * a);

/**********************
 *  GLOBAL FUNCTIONS
 **********************/
lv_obj_t * screen_animations_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    /* ROOT SCREEN OBJECT */
    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "screen_animations_#");
    lv_obj_set_size(lv_obj_0, 480, 320);
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x1a1a1a), 0);

    /***************************
     *        BOTÃO SETA
     ***************************/

    /*
    lv_obj_t * botao_seta =
        button_show_numpad_create(lv_obj_0, seta_esquerda, false, false);

    lv_obj_set_name(botao_seta, "botao_seta");
    lv_obj_set_align(botao_seta, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_pos(botao_seta, -8, -8);
    lv_obj_set_size(botao_seta, 70, 30);
    lv_obj_clear_flag(botao_seta, LV_OBJ_FLAG_CHECKABLE);

    */

    /***************************
     *        LISTA (TOP LAYER)
     ***************************/
    lv_obj_t * button_list = list_create(lv_layer_top());
    lv_obj_set_name(button_list, "button_list");
    lv_obj_set_size(button_list, 220, 300);
    /* Posiciona inicialmente FORA da tela à direita */
    lv_obj_set_pos(button_list, 480, 10);

    lv_obj_add_flag(button_list, LV_OBJ_FLAG_HIDDEN);

    /***************************
     *          VISOR
     ***************************/
    lv_obj_t * visor = visor_create(lv_obj_0);
    lv_obj_set_name(visor, "visor");
    lv_obj_set_align(visor, LV_ALIGN_TOP_MID);
    lv_obj_set_pos(visor, 0, 10);
    lv_obj_set_size(visor, 360, 140);

    /***************************
     *           ARC
     ***************************/
    lv_obj_t * arc_widget = arc_create(lv_obj_0);
    lv_obj_set_name(arc_widget, "arc_widget");
    lv_obj_set_align(arc_widget, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_x(arc_widget, 30);
    lv_obj_set_y(arc_widget, -10);

    arc_bind_to_visor();

    /***************************
     *        TIMELINE
     ***************************/
    lv_anim_timeline_t ** at_array =
        lv_malloc(sizeof(lv_anim_timeline_t *) * _SCREEN_ANIMATIONS_TIMELINE_CNT);

    at_array[SCREEN_ANIMATIONS_TIMELINE_SCREEN_OPEN] =
        timeline_screen_open_create(lv_obj_0);

    lv_obj_set_user_data(lv_obj_0, at_array);
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array);

    /* CALLBACK DO BOTÃO SETA */
    //lv_obj_add_event_cb(botao_seta, botao_seta_toggle_cb, LV_EVENT_CLICKED, NULL);

    /* CALLBACK DE SWIPE */
    lv_obj_add_event_cb(lv_obj_0, swipe_event_cb, LV_EVENT_GESTURE, NULL);

    LV_TRACE_OBJ_CREATE("finished");
    return lv_obj_0;
}

/**********************
 *  TIMELINE HANDLING
 **********************/
lv_anim_timeline_t *
screen_animations_get_timeline(lv_obj_t * obj,
                               screen_animations_timeline_t timeline_id)
{
    if (timeline_id >= _SCREEN_ANIMATIONS_TIMELINE_CNT) {
        LV_LOG_WARN("screen_animations has no timeline with %d ID", timeline_id);
        return NULL;
    }

    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

static lv_anim_timeline_t * timeline_screen_open_create(lv_obj_t * obj)
{
    return lv_anim_timeline_create();
}

static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);

    for (uint32_t i = 0; i < _SCREEN_ANIMATIONS_TIMELINE_CNT; i++)
        lv_anim_timeline_delete(at_array[i]);

    lv_free(at_array);
}

/**********************
 *   CALLBACK DO BOTÃO
 **********************/
static void botao_seta_toggle_cb(lv_event_t * e)
{
    lv_obj_t * list = lv_obj_find_by_name(lv_layer_top(), "button_list");

    if (lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
        open_list_with_anim(list);
    else
        close_list_with_anim(list);
}

/***************************************************
 *     ANIMAÇÃO DA LISTA
 ***************************************************/
static void anim_list_x(lv_obj_t * obj, int32_t v)
{
    lv_obj_set_x(obj, v);
}

static void open_list_with_anim(lv_obj_t * list)
{
    lv_obj_clear_flag(list, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    lv_anim_set_values(&a,
                       480,         /* posição inicial fora da tela */
                       260);        /* posição final dentro */

    lv_anim_start(&a);
}

/* NOVO: callback correto para esconder ao finalizar */
static void hide_list_ready_cb(lv_anim_t * a)
{
    lv_obj_t * list = lv_anim_get_user_data(a);
    lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);
}

static void close_list_with_anim(lv_obj_t * list)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);

    lv_anim_set_values(&a,
                       lv_obj_get_x(list),  /* posição atual */
                       480);                /* sai da tela */

    /* CORRETO: esconde só quando a animação termina */
    lv_anim_set_ready_cb(&a, hide_list_ready_cb);
    lv_anim_set_user_data(&a, list);

    lv_anim_start(&a);
}

/***************************************************
 *     SWIPE EVENT (GESTURE)
 ***************************************************/
static void swipe_event_cb(lv_event_t * e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_obj_t * list = lv_obj_find_by_name(lv_layer_top(), "button_list");

    if (!list) return;

    if (dir == LV_DIR_LEFT) {
        if (lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
            open_list_with_anim(list);
    }
    else if (dir == LV_DIR_RIGHT) {
        if (!lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
            close_list_with_anim(list);
    }
}
