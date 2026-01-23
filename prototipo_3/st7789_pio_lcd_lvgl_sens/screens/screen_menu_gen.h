/**
 * @file screen_menu_gen.h
 * @brief Interface pública da tela de menu
 *
 * Neste arquivo eu declaro a interface pública da tela de menu
 * da aplicação. Aqui ficam expostas apenas as funções necessárias
 * para criação e acesso à tela, mantendo os detalhes de
 * implementação encapsulados no arquivo .c correspondente.
 */

#ifndef SCREEN_MENU_GEN_H
#define SCREEN_MENU_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/* Inclusão do LVGL compatível com diferentes estruturas
 * de projeto (include simples ou caminho relativo) */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../lvgl/lvgl.h"
#endif

/*********************
 * GLOBAL PROTOTYPES
 *********************/

/**
 * @brief Cria (ou reutiliza) a tela de menu
 *
 * Esta função cria a tela de menu da aplicação, adicionando
 * seus componentes visuais e callbacks. Caso a tela já tenha
 * sido criada anteriormente, eu retorno o ponteiro existente
 * para evitar recriação e consumo desnecessário de memória.
 *
 * @return Ponteiro para o objeto raiz da tela de menu
 */
lv_obj_t * screen_menu_create(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCREEN_MENU_GEN_H */
