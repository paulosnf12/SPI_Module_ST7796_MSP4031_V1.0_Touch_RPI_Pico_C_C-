#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <stdbool.h>

// Variáveis compartilhadas
extern char wifi_ssid[32];
extern char wifi_pass[64];
extern volatile bool wifi_connect_requested; // Flag de comando

// Função principal do Wi-Fi
void wifi_task(void);

#endif