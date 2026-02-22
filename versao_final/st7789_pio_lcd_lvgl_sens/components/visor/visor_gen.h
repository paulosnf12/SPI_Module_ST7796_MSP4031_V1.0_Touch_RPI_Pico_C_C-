/**
 * @file visor_gen.h
 * @brief Interface pública do visor de potência óptica
 */

#ifndef VISOR_GEN_H
#define VISOR_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../../lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/* Dimensões do display */
#define DISP_W   480
#define DISP_H   320

/* Altura ocupada pelo visor (metade superior) */
#define VISOR_HEIGHT  160

/* Limites da escala */
#define VISOR_DBM_MIN  (-17.0f)
#define VISOR_DBM_MAX  (  3.0f)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Criação */
lv_obj_t * visor_create(lv_obj_t * parent);

/* Atualização de valor */
void visor_set_value_float(float mw);
void visor_set_value(const char * text);

/* Entrada manual */
void visor_set_enter_callback(void (*cb)(float));
void visor_manual_enter(float mw);

/* Leitura do valor atual */
float visor_get_current_value(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VISOR_GEN_H */
/**
 * @file visor_gen.c
 * @brief Implementação do visor de potência óptica
 *
 * Este componente gráfico implementa um visor de potência
 * óptica medido em dBm e mW. O visor exibe o valor através
 * de uma escala semicircular com ponteiro, além de labels
 * que indicam o valor numérico e a unidade correspondente.
 */