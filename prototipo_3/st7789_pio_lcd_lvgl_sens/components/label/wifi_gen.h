/**
 * @file wifi_gen.h
 * @brief Componente de Label para exibir Horário RTC (sincronizado via Wi-Fi)
 */

#ifndef WIFI_GEN_H
#define WIFI_GEN_H

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
 *  GLOBAL PROTOTYPES
 *********************/

/**
 * @brief Cria a label de relógio/status na tela pai.
 * @param parent Objeto pai onde a label será criada.
 * @return Ponteiro para o objeto label criado.
 */
lv_obj_t * wifi_label_create(lv_obj_t * parent);

/**
 * @brief Atualiza o texto da label com os dados atuais do RTC.
 * Deve ser chamada periodicamente (ex: em um lv_timer).
 */
void wifi_label_update(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WIFI_GEN_H */