/**
 * @file button_back_gen.h
 * @brief Componente BUTTON BACK – Interface pública do botão de voltar
 *
 * Declara a API pública do componente BUTTON BACK, responsável
 * por criar um botão circular de navegação com ícone de retorno.
 * O comportamento (eventos) é definido externamente pelo usuário.
 */

#ifndef BUTTON_BACK_GEN_H
#define BUTTON_BACK_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/

/*
 * Inclusão condicional do LVGL para compatibilidade com
 * diferentes estruturas de projeto e build systems.
 */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../lvgl/lvgl.h"
#endif

/*********************
 * GLOBAL PROTOTYPES
 *********************/

/**
 * @brief Cria um botão de voltar (BACK)
 *
 * Cria um botão circular com ícone direcional à esquerda,
 * estilizado de forma consistente com o restante da UI.
 *
 * O callback fornecido será registrado para eventos do LVGL,
 * normalmente `LV_EVENT_CLICKED`.
 *
 * @param parent   Objeto pai onde o botão será criado
 * @param event_cb Callback de evento do LVGL (pode ser NULL)
 *
 * @return lv_obj_t* Ponteiro para o botão criado
 */
lv_obj_t * button_back_create(lv_obj_t * parent,
                              lv_event_cb_t event_cb);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BUTTON_BACK_GEN_H */
