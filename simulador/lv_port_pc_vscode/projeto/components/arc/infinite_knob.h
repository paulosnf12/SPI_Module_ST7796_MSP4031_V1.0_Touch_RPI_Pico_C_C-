/**
 * @file infinite_knob.h
 */

#ifndef INFINITE_KNOB_H
#define INFINITE_KNOB_H

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

/* (adicione defines aqui se quiser futuramente) */


/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Cria o knob infinito.
 *
 * @param parent Objeto pai onde o knob será inserido.
 * @return lv_obj_t* Ponteiro do objeto criado.
 */
lv_obj_t * infinite_knob_create(lv_obj_t * parent);

/**
 * @brief Faz o bind do knob com o visor (para atualizar valores).
 */
void infinite_knob_bind_to_visor(void);

void arc_set_sensitivity(float s);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INFINITE_KNOB_H */
