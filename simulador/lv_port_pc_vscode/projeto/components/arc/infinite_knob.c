/**
 * @file infinite_knob.c
 * @brief Knob rotativo infinito trabalhando NATIVAMENTE em mW
 */

#include "infinite_knob.h"
#include "../visor/visor_gen.h"
#include <stdio.h>
#include <math.h>
#include "lvgl.h"

/*********************
 * CONSTANTES
 *********************/
#define PI 3.14159265358979323846f

/* Limites em mW */
#define MW_MIN 0.02f
#define MW_MAX 2.0f

/* Sensibilidade: mW por grau */
#define KNOB_SENSITIVITY 0.001f

#define KNOB_RADIUS_OUTER 70
#define KNOB_RADIUS_INNER 40
#define ARC_ROTATION_OFFSET 90

/*********************
 * VARIÁVEIS
 *********************/
static float current_value_mw = 0.1f;
static lv_obj_t *arc_obj = NULL;

static float last_angle = 0.0f;
static bool is_dragging = false;

/*********************
 * PROTÓTIPOS
 *********************/
static void visor_enter_cb(float mw);
static void arc_event_cb(lv_event_t *e);
static float calculate_touch_angle(lv_obj_t *obj, lv_point_t *p);
static bool is_touch_in_ring(lv_obj_t *obj, lv_point_t *p);

/*********************
 * CREATE
 *********************/
lv_obj_t *infinite_knob_create(lv_obj_t *parent)
{
    arc_obj = lv_arc_create(parent);
    lv_obj_set_size(arc_obj, 140, 140);
    lv_obj_center(arc_obj);

    lv_arc_set_bg_angles(arc_obj, 0, 360);
    lv_arc_set_range(arc_obj, 0, 360);
    lv_arc_set_rotation(arc_obj, 270);

    lv_obj_set_style_arc_width(arc_obj, 25, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_obj, 25, LV_PART_INDICATOR);

    lv_obj_add_flag(arc_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(arc_obj, arc_event_cb, LV_EVENT_ALL, NULL);

    lv_arc_set_value(arc_obj, 0);

    return arc_obj;
}

void infinite_knob_bind_to_visor(void)
{
    visor_set_enter_callback(visor_enter_cb);
}

/*********************
 * CALLBACK DO VISOR
 *********************/
static void visor_enter_cb(float mw)
{
    if (mw > MW_MAX) mw = MW_MAX;
    if (mw < MW_MIN) mw = MW_MIN;

    current_value_mw = mw;

    visor_set_value_float(current_value_mw);
    lv_arc_set_value(arc_obj, 0);
}

/*********************
 * UTILITÁRIOS
 *********************/
static bool is_touch_in_ring(lv_obj_t *obj, lv_point_t *p)
{
    lv_area_t a;
    lv_obj_get_coords(obj, &a);

    float cx = a.x1 + lv_area_get_width(&a) / 2.0f;
    float cy = a.y1 + lv_area_get_height(&a) / 2.0f;

    float dx = p->x - cx;
    float dy = p->y - cy;

    float dist = sqrtf(dx * dx + dy * dy);
    return (dist >= KNOB_RADIUS_INNER && dist <= KNOB_RADIUS_OUTER);
}

static float calculate_touch_angle(lv_obj_t *obj, lv_point_t *p)
{
    lv_area_t a;
    lv_obj_get_coords(obj, &a);

    float cx = a.x1 + lv_area_get_width(&a) / 2.0f;
    float cy = a.y1 + lv_area_get_height(&a) / 2.0f;

    float dx = p->x - cx;
    float dy = p->y - cy;

    float ang = atan2f(dy, dx) * (180.0f / PI);
    if (ang < 0) ang += 360.0f;
    return ang;
}

/*********************
 * EVENT HANDLER
 *********************/
static void arc_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);

    if (code == LV_EVENT_PRESSED)
    {
        if (!is_touch_in_ring(obj, &p))
        {
            is_dragging = false;
            return;
        }

        last_angle = calculate_touch_angle(obj, &p);
        is_dragging = true;
        return;
    }

    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        is_dragging = false;
        return;
    }

    if (code == LV_EVENT_PRESSING && is_dragging)
    {
        float angle = calculate_touch_angle(obj, &p);
        float diff = angle - last_angle;

        if (diff < -180.0f) diff += 360.0f;
        if (diff > 180.0f)  diff -= 360.0f;

        last_angle = angle;
        if (fabsf(diff) < 0.2f) return;

        float novo_mw = current_value_mw + diff * KNOB_SENSITIVITY;

        if (novo_mw > MW_MAX) novo_mw = MW_MAX;
        if (novo_mw < MW_MIN) novo_mw = MW_MIN;

        if (novo_mw != current_value_mw)
        {
            current_value_mw = novo_mw;
            visor_set_value_float(current_value_mw);
        }

        float visual_angle = angle + ARC_ROTATION_OFFSET;
        if (visual_angle >= 360.0f) visual_angle -= 360.0f;

        lv_arc_set_value(obj, (int)visual_angle);
    }
}
