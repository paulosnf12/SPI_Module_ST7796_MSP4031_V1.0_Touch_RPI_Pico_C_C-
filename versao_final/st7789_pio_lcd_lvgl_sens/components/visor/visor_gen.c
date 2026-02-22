/**
 * @file visor_gen.c
 * @brief Visor – Medidor de Potência Óptica (mW / dBm)
 *
 * Este módulo implementa um visor analógico/digital para medição de potência
 * óptica, exibindo:
 *  - Escala semicircular em dBm
 *  - Ponteiro analógico
 *  - Valor digital principal (mW / uW / nW)
 *  - Valor secundário em dBm
 *
 * Desenvolvido para LVGL em display 480x320,
 * ocupando a metade superior da tela.
 */

#include "visor_gen.h"
#include "lvgl.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/**********************
 * DEFINES
 **********************/

/* Raio da escala circular */
#define SCALE_RADIUS        200

/* Limites da escala em dBm (vindos da interface pública) */
#define DBM_MIN             VISOR_DBM_MIN
#define DBM_MAX             VISOR_DBM_MAX

/* Ângulo inicial e varredura da escala (em graus) */
#define ARC_START_ANGLE     210.0f
#define ARC_SWEEP_ANGLE     120.0f

/* Constante PI para cálculo trigonométrico */
#define PI                  3.14159265359f

/**********************
 * CORES (tema do visor)
 **********************/
#define COL_BG     0x1E1E2E
#define COL_WHITE  0xFFFFFF
#define COL_GRAY   0x2A2A3A
#define COL_ACCENT 0x1E90FF

/**********************
 * OBJETOS LVGL
 **********************/

/* Escala semicircular */
static lv_obj_t * scale_obj;

/* Ponteiro analógico */
static lv_obj_t * needle;

/* Centro do ponteiro (hub visual) */
static lv_obj_t * hub;

/* Label do valor principal (potência linear) */
static lv_obj_t * label_value;

/* Unidade do valor principal (mW / uW / nW) */
static lv_obj_t * label_unit;

/* Valor numérico em dBm */
static lv_obj_t * label_dbm_value;

/* Título fixo "dBm" */
static lv_obj_t * label_dbm_title;

/* Pontos do ponteiro (linha) */
static lv_point_precise_t needle_pts[2];

/* Valor real armazenado (em mW) */
static float current_mw = 0.0f;

/* Callback opcional para entrada manual */
static void (*enter_callback)(float) = NULL;

/**********************
 * UTIL – Conversão float → string
 *
 * Implementação própria para evitar %f
 * (útil em ambientes embarcados)
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
    /* Container raiz do visor */
    lv_obj_t * root = lv_obj_create(parent);
    lv_obj_set_size(root, DISP_W, VISOR_HEIGHT);
    lv_obj_set_style_bg_opa(root, LV_OPA_0, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /******** Escala ********/
    scale_obj = lv_scale_create(root);
    lv_obj_set_size(scale_obj,
                    SCALE_RADIUS * 2,
                    SCALE_RADIUS * 2);

    /* Posiciona a escala parcialmente fora do container,
       criando efeito de semicírculo */
    lv_obj_align(scale_obj,
                 LV_ALIGN_TOP_MID,
                 0,
                 VISOR_HEIGHT - SCALE_RADIUS + 50);

    lv_scale_set_mode(scale_obj, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale_obj, (int)DBM_MIN, (int)DBM_MAX);
    lv_scale_set_rotation(scale_obj, ARC_START_ANGLE);
    lv_scale_set_angle_range(scale_obj, ARC_SWEEP_ANGLE);
    lv_scale_set_total_tick_count(scale_obj, 21);
    lv_scale_set_major_tick_every(scale_obj, 5);

    /* Estilo dos ticks */
    lv_obj_set_style_line_color(scale_obj,
                               lv_color_hex(COL_GRAY),
                               LV_PART_ITEMS);

    lv_obj_set_style_line_color(scale_obj,
                               lv_color_hex(COL_WHITE),
                               LV_PART_INDICATOR);

    lv_obj_set_style_length(scale_obj, 14, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale_obj, 7,  LV_PART_ITEMS);

    lv_obj_set_style_text_font(scale_obj,
                               &lv_font_montserrat_14,
                               LV_PART_INDICATOR);

    lv_obj_set_style_text_color(scale_obj,
                               lv_color_hex(COL_WHITE),
                               LV_PART_INDICATOR);

    /******** Ponteiro ********/
    needle = lv_line_create(root);
    lv_obj_set_style_line_width(needle, 3, 0);
    lv_obj_set_style_line_color(needle,
                               lv_color_hex(COL_ACCENT),
                               0);
    lv_obj_set_style_line_rounded(needle, true, 0);

    /******** Hub (centro do ponteiro) ********/
    hub = lv_obj_create(root);
    lv_obj_set_size(hub, 14, 14);
    lv_obj_set_style_radius(hub,
                            LV_RADIUS_CIRCLE,
                            0);
    lv_obj_set_style_bg_color(hub,
                              lv_color_hex(COL_ACCENT),
                              0);
    lv_obj_set_style_border_width(hub, 0, 0);

    /******** Label título dBm ********/
    label_dbm_title = lv_label_create(root);
    lv_label_set_text(label_dbm_title, "dBm");
    lv_obj_set_style_text_font(label_dbm_title,
                               &lv_font_montserrat_18,
                               0);
    lv_obj_set_style_text_color(label_dbm_title,
                               lv_color_hex(COL_GRAY),
                               0);
    lv_obj_align(label_dbm_title,
                 LV_ALIGN_TOP_MID,
                 0,
                 40);

    /******** Valor dBm ********/
    label_dbm_value = lv_label_create(root);
    lv_obj_set_style_text_font(label_dbm_value,
                               &lv_font_montserrat_20,
                               0);
    lv_obj_set_style_text_color(label_dbm_value,
                               lv_color_hex(COL_WHITE),
                               0);
    lv_label_set_text(label_dbm_value, "0.00");
    lv_obj_align(label_dbm_value,
                 LV_ALIGN_TOP_MID,
                 0,
                 62);

    /******** Valor principal ********/
    label_value = lv_label_create(root);
    lv_obj_set_style_text_font(label_value,
                               &lv_font_montserrat_48,
                               0);
    lv_obj_set_style_text_color(label_value,
                               lv_color_hex(COL_WHITE),
                               0);
    lv_label_set_text(label_value, "0.000");
    lv_obj_align(label_value,
                 LV_ALIGN_TOP_MID,
                 0,
                 88);

    /******** Unidade ********/
    label_unit = lv_label_create(root);
    lv_obj_set_style_text_font(label_unit,
                               &lv_font_montserrat_20,
                               0);
    lv_obj_set_style_text_color(label_unit,
                               lv_color_hex(COL_ACCENT),
                               0);
    lv_label_set_text(label_unit, "mW");
    lv_obj_align_to(label_unit,
                    label_value,
                    LV_ALIGN_OUT_RIGHT_BOTTOM,
                    6,
                    -6);

    // --- CORREÇÃO INÍCIO ---

    // 1. Força o LVGL a calcular as posições e tamanhos (necessário para o cálculo do centro da agulha)
    lv_obj_update_layout(root);

    // 2. Define um valor inicial (0.0f) para que a agulha seja desenhada e os textos alinhados
    // Isso fará a trigonometria rodar e tirará a agulha do canto (0,0)
    visor_set_value_float(0.0f);

    // --- CORREÇÃO FIM ---

    return root;
}

/**********************
 * ATUALIZAÇÃO DO VISOR
 **********************/
void visor_set_value_float(float mw)
{
    /* Segurança */
    if (!scale_obj || !needle)
        return;

    /* Evita log10(0) */
    if (mw < 1e-12f)
        mw = 1e-12f;

    current_mw = mw;

    /* Conversão para dBm */
    float dbm = 10.0f * log10f(mw);

    /* Saturação visual */
    float dbm_c = dbm;
    if (dbm_c < DBM_MIN) dbm_c = DBM_MIN;
    if (dbm_c > DBM_MAX) dbm_c = DBM_MAX;

    /* Mapeamento linear → ângulo */
    float pct = (dbm_c - DBM_MIN) / (DBM_MAX - DBM_MIN);
    float angle = ARC_START_ANGLE + pct * ARC_SWEEP_ANGLE;
    float rad = angle * (PI / 180.0f);

    /* Centro da escala */
    lv_coord_t cx =
        lv_obj_get_x(scale_obj) +
        lv_obj_get_width(scale_obj) / 2;

    lv_coord_t cy =
        lv_obj_get_y(scale_obj) +
        lv_obj_get_height(scale_obj) / 2;

    /* Comprimento do ponteiro */
    float len = SCALE_RADIUS - 40;

    /* Atualiza geometria do ponteiro */
    needle_pts[0].x = cx;
    needle_pts[0].y = cy;
    needle_pts[1].x = cx + len * cosf(rad);
    needle_pts[1].y = cy + len * sinf(rad);

    lv_line_set_points(needle, needle_pts, 2);
    lv_obj_set_pos(hub, cx - 7, cy - 7);

    /******** Atualização de labels ********/
    char buf[16];

    float_to_str(buf, dbm, 2);
    lv_label_set_text(label_dbm_value, buf);

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
    lv_obj_align_to(label_unit,
                    label_value,
                    LV_ALIGN_OUT_RIGHT_BOTTOM,
                    6,
                    -6);
}

/**********************
 * FUNÇÕES AUXILIARES
 **********************/
void visor_set_value(const char * text)
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
