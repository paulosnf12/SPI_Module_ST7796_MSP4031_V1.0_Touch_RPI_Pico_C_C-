#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "pico/multicore.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// Includes NTP
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

// Ponte com o LVGL
#include "wifi_handler.h"

// Definição REAL das variáveis globais (aqui elas ocupam memória)
char wifi_ssid[32] = {0};
char wifi_pass[64] = {0};
volatile bool wifi_connect_requested = false;
static bool is_connected = false; // Controle local de estado

// Configurações NTP
#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800 
#define TIMEZONE_OFFSET -3 

// Buffer HTTP
char http_response[1024];

// ==========================================
// MÓDULO HTTP (Igual ao anterior)
// ==========================================
void create_http_response() {
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n"
        "<!DOCTYPE html><html><body><h1>Raspberry Pi Pico W</h1><p>Status: Conectado via LVGL!</p></body></html>\r\n");
}

static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) { tcp_close(tpcb); return ERR_OK; }
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) return;
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) return;
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("[HTTP] Servidor iniciado na porta 80\n");
}

// ==========================================
// MÓDULO NTP / RTC (Igual ao anterior)
// ==========================================
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);
    if (port == NTP_PORT && p->tot_len == NTP_MSG_LEN && mode == 4 && stratum != 0) {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t raw_time = seconds_since_1970 + (TIMEZONE_OFFSET * 3600);
        struct tm *utc = gmtime(&raw_time);
        datetime_t t = { .year = utc->tm_year + 1900, .month = utc->tm_mon + 1, .day = utc->tm_mday,
                         .dotw = utc->tm_wday, .hour = utc->tm_hour, .min = utc->tm_min, .sec = utc->tm_sec };
        rtc_init();
        rtc_set_datetime(&t);
        printf("[NTP] Sincronizado: %02d:%02d:%02d\n", t.hour, t.min, t.sec);
    }
    pbuf_free(p);
}

static void ntp_request(struct udp_pcb *pcb, const ip_addr_t *addr) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *) p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(pcb, p, addr, NTP_PORT);
    pbuf_free(p);
}

static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    struct udp_pcb *pcb = (struct udp_pcb *)arg;
    if (ipaddr) ntp_request(pcb, ipaddr);
}

void sync_rtc_with_ntp() {
    struct udp_pcb *pcb = udp_new();
    if (!pcb) return;
    udp_recv(pcb, ntp_recv, NULL);
    ip_addr_t ntp_server_ip;
    int err = dns_gethostbyname(NTP_SERVER, &ntp_server_ip, ntp_dns_found, pcb);
    if (err == ERR_OK) ntp_request(pcb, &ntp_server_ip);
}

// ==========================================
// TAREFA PRINCIPAL (CORE 1)
// ==========================================
void wifi_task() {
    // Inicializa Hardware Wi-Fi
    if (cyw43_arch_init()) {
        printf("[Wi-Fi] Erro ao inicializar hardware.\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); // LED apagado

    printf("[Wi-Fi] Aguardando credenciais do LVGL...\n");

    while (true) {
        
        // --- MOMENTO DE CONEXÃO ---
        // Se o usuário apertou o botão no display:
        if (wifi_connect_requested) {
            printf("[Wi-Fi] Tentando conectar em: %s\n", wifi_ssid);
            
            // Desliga LED e reseta estado
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            is_connected = false;

            // Tenta conectar
            int err = cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_pass, CYW43_AUTH_WPA2_AES_PSK, 15000);

            if (err == 0) {
                printf("[Wi-Fi] Conectado!\n");
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // LED Verde
                
                // Inicia serviços
                start_http_server();
                sync_rtc_with_ntp();
                
                is_connected = true; // Marca como conectado
            } else {
                printf("[Wi-Fi] Falha (%d). Verifique a senha.\n", err);
                // Pisca LED indicando erro
                for(int i=0; i<3; i++) {
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); sleep_ms(200);
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); sleep_ms(200);
                }
            }
            
            // Reseta a flag para não tentar conectar repetidamente
            wifi_connect_requested = false;
        }

        // --- MOMENTO DE MANUTENÇÃO (Se já estiver conectado) ---
        if (is_connected) {
            cyw43_arch_poll(); // Mantém Wi-Fi ativo

            static uint64_t last_print = 0;
            if (time_us_64() - last_print > 1000000) { // A cada 1 seg
                datetime_t t;
                if (rtc_get_datetime(&t)) {
                    printf("[RTC] %02d:%02d:%02d\n", t.hour, t.min, t.sec);
                }
                last_print = time_us_64();
            }
        }
        sleep_ms(50); // Pausa para não fritar a CPU
    }
}