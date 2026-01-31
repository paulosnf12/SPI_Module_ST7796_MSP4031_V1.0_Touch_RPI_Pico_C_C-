/**
 * @file wifi_gen.c
 */

#include "wifi_gen.h"
#include <stdio.h>

// Includes específicos do Hardware (Raspberry Pi Pico)
// Necessários para ler as variáveis do RTC que o wifi_handler.c configurou
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

/*********************
 *  STATIC VARIABLES
 *********************/
static lv_obj_t * time_label = NULL;

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

lv_obj_t * wifi_label_create(lv_obj_t * parent)
{
    // Cria a label no objeto pai
    time_label = lv_label_create(parent);

    // Configuração inicial do texto
    lv_label_set_text(time_label, "--:--:--");

    // Estilização (Baseado no tema escuro do seu PDF)
    // Usando fonte média/pequena para caber no canto
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0); // Branco
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_14, 0); // Ajuste conforme suas fontes disponíveis

    // Posicionamento: Canto Superior Direito
    // O Logo HWIT está no Topo Esquerdo (page 64 do PDF)
    // O Visor está no Topo Centro (page 63 do PDF)
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -10, 10);

    return time_label;
}

void wifi_label_update(void)
{
    if (time_label == NULL) return;

    datetime_t t;
    bool rtc_ok = rtc_get_datetime(&t);

    if (rtc_ok) {
        // Formata: HH:MM:SS
        lv_label_set_text_fmt(time_label, "%02d:%02d:%02d", t.hour, t.min, t.sec);
    } else {
        // Caso o RTC não esteja rodando ainda
        lv_label_set_text(time_label, "--:--");
    }
}