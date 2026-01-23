/**
 * @file time_gen.h
 * @brief Interface pública do widget de horário
 *
 * Este arquivo define a API pública do componente de horário,
 * utilizado no menu principal para acesso à tela de
 * configuração de Wi-Fi e ajuste de horário.
 */

#ifndef TIME_GEN_H
#define TIME_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/

/*
 * Inclusão do LVGL de forma compatível com diferentes estruturas
 * de projeto (LVGL local ou externo).
 */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../../lvgl/lvgl.h"
#endif

/*********************
 * GLOBAL PROTOTYPES
 *********************/

/**
 * @brief Cria o widget de horário no menu principal
 *
 * Esta função cria um botão interativo que representa
 * o acesso à configuração de horário.
 *
 * @param parent Objeto pai onde o widget será inserido
 * @return Ponteiro para o objeto LVGL criado
 */
lv_obj_t * time_create(lv_obj_t * parent);

/**
 * @brief Define manualmente o horário do sistema
 *
 * Função chamada pela tela de ajuste de horário para
 * atualizar os valores internos de hora e minuto.
 *
 * @param hour   Hora no formato 24h (0–23)
 * @param minute Minuto (0–59)
 */
void time_set_manual(int hour, int minute);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TIME_GEN_H */
/**
 * @file time_gen.c
 * @brief Implementação do widget de horário no menu principal
 *
 * Este componente implementa o botão de horário exibido
 * no menu principal da aplicação. Ao ser clicado, ele
 * abre a tela de configuração de Wi-Fi e ajuste de horário.
 */