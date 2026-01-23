/**
 * @file screen_animations_gen.h
 * @brief Interface da tela principal com animações
 *
 * Neste arquivo eu declaro a interface pública da tela principal
 * da aplicação. Aqui ficam expostas apenas as funções e tipos
 * necessários para criação da tela e acesso às suas timelines
 * de animação, mantendo a implementação encapsulada no .c.
 */

#ifndef SCREEN_ANIMATIONS_H
#define SCREEN_ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/* Inclusão do LVGL de forma compatível com diferentes estruturas
 * de projeto (include simples ou caminho relativo) */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../lvgl/lvgl.h" 
#endif

/*********************
 *      DEFINES
 *********************/
/* Não há defines públicos para esta tela no momento */

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Identificadores das timelines da tela principal
 *
 * Este enum define todas as timelines disponíveis para a
 * screen_animations. Ele é utilizado para recuperar uma
 * timeline específica de forma segura e legível.
 */
typedef enum {
    /* Timeline responsável pela animação de abertura da tela */
    SCREEN_ANIMATIONS_TIMELINE_SCREEN_OPEN = 0,

    /* Quantidade total de timelines desta tela
     * (usado internamente para controle e liberação de memória) */
    _SCREEN_ANIMATIONS_TIMELINE_CNT = 1
} screen_animations_timeline_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Cria (ou reutiliza) a tela principal com animações
 *
 * Esta função cria a tela principal da aplicação, incluindo
 * todos os seus componentes visuais e callbacks. Caso a tela
 * já tenha sido criada anteriormente, eu apenas retorno o
 * ponteiro existente para evitar recriação desnecessária.
 *
 * @return Ponteiro para o objeto raiz da tela
 */
lv_obj_t * screen_animations_create(void);

/**
 * @brief Retorna uma timeline associada à tela principal
 *
 * Utilizo esta função para acessar uma timeline específica
 * da screen_animations, normalmente para iniciar ou controlar
 * animações a partir de outros módulos da aplicação.
 *
 * @param obj         Ponteiro para o objeto da tela screen_animations
 * @param timeline_id Identificador da timeline desejada
 *
 * @return Ponteiro para a timeline ou NULL caso não exista
 */
lv_anim_timeline_t *
screen_animations_get_timeline(lv_obj_t * obj,
                               screen_animations_timeline_t timeline_id);

/**********************
 *      MACROS
 *********************/
/* Nenhuma macro pública definida */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCREEN_ANIMATIONS_H */
