/**
 * @file visor_gen.c
 * @brief Visor – Medidor de Potência Óptica (mW / dBm)
 */

#include "visor_gen.h"
#include "lvgl.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**********************
 * DEFINES
 **********************/
#define SCALE_RADIUS        150

#define DBM_MIN            (-17.0f)
#define DBM_MAX            (  3.0f)

#define ARC_START_ANGLE     210.0f
#define ARC_SWEEP_ANGLE     120.0f

#define PI                 3.14159265359f
#define VISOR_H            (DISP_H / 2)

#define NEEDLE_Y_OFFSET   12




/**********************
 * CORES
 **********************/
#define COL_BG     0x1E1E2E
#define COL_WHITE  0xFFFFFF
#define COL_GRAY   0x2A2A3A
#define COL_LIGHT_GRAY 0xAAAAAA
#define COL_ACCENT 0x1E90FF

/**********************
 * OBJETOS
 **********************/
static lv_obj_t *scale_obj;
static lv_obj_t *needle;
static lv_obj_t *hub;
static lv_obj_t *label_value;
static lv_obj_t *label_unit;
static lv_obj_t *label_dbm;

static lv_point_precise_t needle_pts[2];

/* Valor REAL armazenado (mW) */
static float current_mw = 0.0f;

/* Callback ENTER */
static void (*enter_callback)(float) = NULL;

/**********************
 * UTIL – float → string (sem %f)
 **********************/
static void float_to_str(char *buf, float val, int dec)
{
    int ip = (int)val;
    int fp = (int)((fabsf(val) - abs(ip)) * powf(10, dec));
    lv_snprintf(buf, 16, "%d.%0*d", ip, dec, fp);
}

/**********************
 * CRIAÇÃO DO VISOR
 **********************/
lv_obj_t * visor_create(lv_obj_t * parent)
{
    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_set_size(root, DISP_W, VISOR_H );
    lv_obj_set_style_bg_opa(root, LV_OPA_0, 0);
    lv_obj_set_style_border_width(root, 0, 0);

    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /* Escala */
    scale_obj = lv_scale_create(root);
    lv_obj_set_size(scale_obj, SCALE_RADIUS * 2, SCALE_RADIUS * 2);
    lv_obj_align(scale_obj, LV_ALIGN_TOP_MID, 0, 10);

    lv_scale_set_mode(scale_obj, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale_obj, (int)DBM_MIN, (int)DBM_MAX);
    lv_scale_set_rotation(scale_obj, ARC_START_ANGLE);
    lv_scale_set_angle_range(scale_obj, ARC_SWEEP_ANGLE);
    lv_scale_set_total_tick_count(scale_obj, 21);
    lv_scale_set_major_tick_every(scale_obj, 5);

    lv_obj_set_style_line_color(scale_obj, lv_color_hex(COL_LIGHT_GRAY), LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale_obj, lv_color_hex(COL_WHITE), LV_PART_INDICATOR);
    lv_obj_set_style_length(scale_obj, 14, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale_obj, 7,  LV_PART_ITEMS);
    lv_obj_set_style_text_font(scale_obj, &lv_font_montserrat_14, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(scale_obj, lv_color_hex(COL_WHITE), LV_PART_INDICATOR);

    /* Ponteiro */
    needle = lv_line_create(root);
    lv_obj_set_style_line_width(needle, 3, 0);
    lv_obj_set_style_line_color(needle, lv_color_hex(COL_ACCENT), 0);
    lv_obj_set_style_line_rounded(needle, true, 0);

    /* Hub */
    hub = lv_obj_create(root);
    lv_obj_set_size(hub, 14, 14);
    lv_obj_set_style_radius(hub, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(hub, lv_color_hex(COL_ACCENT), 0);
    lv_obj_set_style_border_width(hub, 0, 0);

    /* dBm topo */
    label_dbm = lv_label_create(root);
    lv_obj_set_style_text_font(label_dbm, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(label_dbm, lv_color_hex(COL_WHITE), 0);
    lv_label_set_text(label_dbm, "dBm");
    lv_obj_align(label_dbm, LV_ALIGN_TOP_MID, 0, 53);

    /* Valor principal */
    label_value = lv_label_create(root);
    lv_obj_set_style_text_font(label_value, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(label_value, lv_color_hex(COL_WHITE), 0);
    lv_label_set_text(label_value, "0.000");
    lv_obj_align(label_value, LV_ALIGN_TOP_MID, 0, 70);

    /* Unidade */
    label_unit = lv_label_create(root);
    lv_obj_set_style_text_font(label_unit, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(label_unit, lv_color_hex(COL_ACCENT), 0);
    lv_label_set_text(label_unit, "mW");
    lv_obj_align_to(label_unit, label_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 6, -6);

    return root;
}

/**********************
 * ATUALIZAÇÃO DO VISOR
 **********************/
void visor_set_value_float(float mw)
{
    if (!scale_obj || !needle) return;
    if (mw < 1e-12f) mw = 1e-12f;

    current_mw = mw;

    float dbm = 10.0f * log10f(mw);

    /* Clamp visual */
    float dbm_c = dbm;
    if (dbm_c < DBM_MIN) dbm_c = DBM_MIN;
    if (dbm_c > DBM_MAX) dbm_c = DBM_MAX;

    float pct = (dbm_c - DBM_MIN) / (DBM_MAX - DBM_MIN);
    float angle = ARC_START_ANGLE + pct * ARC_SWEEP_ANGLE;
    float rad = angle * (PI / 180.0f);

    lv_coord_t cx = lv_obj_get_x(scale_obj) + lv_obj_get_width(scale_obj) / 2;
    lv_coord_t cy = lv_obj_get_y(scale_obj) + lv_obj_get_height(scale_obj) / 2;

    float len = SCALE_RADIUS - 40;

    needle_pts[0].x = cx;
    needle_pts[0].y = cy;
    needle_pts[1].x = cx + len * cosf(rad);
    needle_pts[1].y = cy + len * sinf(rad);

    lv_line_set_points(needle, needle_pts, 2);
    lv_obj_set_pos(hub, cx - 7, cy - 7);

    /* Labels */
    char buf[16];

    float_to_str(buf, dbm, 2);
    strcat(buf, " dBm");
    lv_label_set_text(label_dbm, buf);

    if (mw >= 1.0f) {
        float_to_str(buf, mw, 3);
        lv_label_set_text(label_unit, "mW");
    } else if (mw >= 0.001f) {
        float_to_str(buf, mw * 1000.0f, 2);
        lv_label_set_text(label_unit, "uW");
    } else {
        float_to_str(buf, mw * 1000000.0f, 1);
        lv_label_set_text(label_unit, "nW");
    }

    lv_label_set_text(label_value, buf);
    lv_obj_align_to(label_unit, label_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 6, -6);
}

/**********************
 * FUNÇÕES AUXILIARES
 **********************/
void visor_set_value(const char *text)
{
    if (!text) return;
    visor_set_value_float((float)atof(text));
}

float visor_get_current_value(void)
{
    return current_mw;
}

void visor_set_enter_callback(void (*cb)(float))
{
    enter_callback = cb;
}

void visor_manual_enter(float mw)
{
    visor_set_value_float(mw);
    if (enter_callback) {
        enter_callback(mw);
    }
}
