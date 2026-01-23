/**
 * @file home_button.h
 * @brief Interface pública do botão HOME circular
 *
 * Este header define a API para criação de um botão HOME estilizado,
 * encapsulando detalhes visuais e permitindo reutilização em
 * diferentes telas do sistema.
 */

#ifndef HOME_BUTTON_H
#define HOME_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/

/*
 * Inclusão condicional do LVGL:
 * - Compatível com projetos que usam LV_LVGL_H_INCLUDE_SIMPLE
 * - Mantém portabilidade entre estruturas de diretórios
 */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../../lvgl/lvgl.h"
#endif

/*********************
 * PUBLIC API
 *********************/

/**
 * @brief Cria um botão HOME circular com ícone centralizado
 *
 * A função cria um botão LVGL (`lv_btn`) com:
 *  - formato circular
 *  - ícone HOME nativo do LVGL
 *  - estilo visual padronizado
 *
 * O botão não define eventos internamente, permitindo que
 * o comportamento seja configurado externamente.
 *
 * @param parent Objeto pai onde o botão será criado
 * @param size   Diâmetro do botão (largura e altura iguais)
 *
 * @return Ponteiro para o objeto `lv_obj_t` do botão criado
 */
lv_obj_t * home_button_create(lv_obj_t * parent, lv_coord_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HOME_BUTTON_H */
