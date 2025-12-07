/**
 * @file arc_gen.c
 */

#include "arc_gen.h"
#include "../../ui_animations.h"
#include "../visor/visor_gen.h"
#include <stdio.h>
#include <math.h>

static float valor_base = 0.0f;
static lv_obj_t * arc_obj = NULL;

/* Amplitude da variação do ARC */
static float arc_range = 3.0f;

static void visor_enter_cb(float v);
static void arc_event_cb(lv_event_t * e);

/************** CREATE *************/
lv_obj_t * arc_create(lv_obj_t * parent)
{
    // NÃO registrar aqui o callback do visor — bind será feito explicitamente depois
    arc_obj = lv_arc_create(parent);
    lv_obj_set_name(arc_obj, "arc_control");

    lv_arc_set_range(arc_obj, 0, 100);
    lv_arc_set_value(arc_obj, 50); // meio

    lv_obj_add_event_cb(arc_obj, arc_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    return arc_obj;
}

/********** NOVA FUNÇÃO: vincular o ARC ao VISOR (depois de criar os dois) **********/
void arc_bind_to_visor(void)
{
    // registra o callback do visor aqui — garante que o visor já exista
    visor_set_enter_callback(visor_enter_cb);
    printf("[arc] vinculado ao visor (callback registrado)\n");
}

/************** CALLBACKS ************/

// ENTER no visor → salva novo valor base
static void visor_enter_cb(float v)
{
    valor_base = v;        // <-- agora o ARC usa isso como referência
    printf("[arc] Valor base definido: %.2f\n", valor_base);

    // Reset do ARC para o meio (offset = 0)
    if (arc_obj)
        lv_arc_set_value(arc_obj, 50);

    // Atualiza o visor com o valor base imediatamente (opcional redundância)
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f", valor_base);
    visor_set_value(buf);
}

// Mexeu no ARC → atualiza visor
static void arc_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    int raw = lv_arc_get_value(arc);  // 0 a 100

    // Converte para offset -arc_range … +arc_range
    float offset = ((raw - 50) / 50.0f) * arc_range;

    // Agora o valor real depende do valor_base digitado no ENTER
    float novo_valor = valor_base + offset;

    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f", novo_valor);

    visor_set_value(buf);
}

/* Opcional: permitir ajustar dinamicamente o range do ARC */
void arc_set_range(float r)
{
    if (r <= 0) return;
    arc_range = r;
}