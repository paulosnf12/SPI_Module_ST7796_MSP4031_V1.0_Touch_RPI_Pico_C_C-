/**
 * @file infinite_knob.c
 * @brief Knob rotativo infinito trabalhando NATIVAMENTE em mW
 */

#include "infinite_knob.h"
#include "../visor/visor_gen.h"
#include <stdio.h>
#include <math.h>
#include "lvgl.h"

#include "../../ad5592r.h" 

/*********************
 * CONSTANTES
 *********************/
#define PI 3.14159265358979323846f

/* Limites em mW */
#define MW_MIN 0.02f
#define MW_MAX 2.0f

// --- NOVOS LIMITES (DAC 12 BITS) ---
#define DAC_MIN 0
#define DAC_MAX 4095

#define KNOB_DAC_STEP 5  // Cada grau gira 5 bits (rápido)

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

// Agora controlamos INTEIROS (0 a 4095), não mais float mW
static int32_t current_dac_val = 0;

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

    // Inicializa hardware no zero também
    ad5592r_set_dac(DAC_CHANNEL_IO3, 0);

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


// Função auxiliar para mapear float (mW) para uint16 (DAC 0-4095)
static uint16_t map_mw_to_dac(float mw) {
    // MW_MIN = 0.02f, MW_MAX = 2.0f (definidos no .h ou .c original)
    // Se mw <= min -> 0
    // Se mw >= max -> 4095
    
    if (mw <= MW_MIN) return 0;
    if (mw >= MW_MAX) return 4095;

    // Regra de três simples (Linear)
    float percentage = (mw - MW_MIN) / (MW_MAX - MW_MIN);
    return (uint16_t)(percentage * 4095.0f);
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

        // --- LÓGICA DE CONTROLE DIRETO DO DAC ---
        
        // Calcula a mudança baseada no ângulo * sensibilidade
        int change = (int)(diff * KNOB_DAC_STEP); 

        // Atualiza o valor acumulado
        int32_t novo_valor = current_dac_val + change;

        // Limita entre 0 e 4095
        if (novo_valor > DAC_MAX) novo_valor = DAC_MAX;
        if (novo_valor < DAC_MIN) novo_valor = DAC_MIN;

        // Se houve mudança, atualiza hardware
        if (novo_valor != current_dac_val)
        {
            current_dac_val = novo_valor;
            
            // 1. Envia para o Driver AD5592R
            ad5592r_set_dac(DAC_CHANNEL_IO3, (uint16_t)current_dac_val);

            // 2. Printa no terminal para você debugar na BitDogLab
            // Calcula tensão estimada (assumindo 5V de referência)
            float tensao = (current_dac_val * 5.0f) / 4095.0f;
            printf("KNOB GIRANDO: Diff=%.1f | DAC=%4d (0-4095) | V=%.2fV\n", 
                   diff, current_dac_val, tensao);
                }

        float novo_mw = current_value_mw + diff * KNOB_SENSITIVITY;

        if (novo_mw > MW_MAX) novo_mw = MW_MAX;
        if (novo_mw < MW_MIN) novo_mw = MW_MIN;

        if (novo_mw != current_value_mw)
        {
            current_value_mw = novo_mw;
            visor_set_value_float(current_value_mw);

            // 1. Converte o valor de mW para bits do DAC (0 a 4095)
            uint16_t dac_value = map_mw_to_dac(current_value_mw);
            
            // 2. Envia para o hardware (Canal 3, conforme seu exemplo)
            ad5592r_set_dac(DAC_CHANNEL_IO3, dac_value);
            // ------------------------------

            printf("Valor atualizado: %.3f mW -> DAC: %d\n", current_value_mw, dac_value);
        }

        float visual_angle = angle + ARC_ROTATION_OFFSET;
        if (visual_angle >= 360.0f) visual_angle -= 360.0f;

        lv_arc_set_value(obj, (int)visual_angle);
    }
}
