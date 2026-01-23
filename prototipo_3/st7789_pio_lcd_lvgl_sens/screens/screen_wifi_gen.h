/**
 * @file screen_wifi_gen.h
 * @brief Interface da tela de configuração Wi-Fi / horário automático
 *
 * Neste arquivo eu declaro a interface pública da tela de
 * configuração de Wi-Fi. Esta tela é responsável por permitir
 * a configuração de rede sem fio, que futuramente será utilizada
 * para sincronização automática de horário (NTP) e outros serviços
 * dependentes de conectividade.
 */

#ifndef SCREEN_WIFI_GEN_H
#define SCREEN_WIFI_GEN_H

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
 * @brief Cria a tela de configuração Wi-Fi
 *
 * Esta função cria a interface gráfica da tela de Wi-Fi,
 * incluindo campos de entrada, teclado virtual e botões
 * de navegação. A tela é criada sob demanda e não utiliza
 * cache persistente.
 *
 * @return Ponteiro para o objeto raiz da tela de Wi-Fi
 */
lv_obj_t * screen_wifi_create(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCREEN_WIFI_GEN_H */
