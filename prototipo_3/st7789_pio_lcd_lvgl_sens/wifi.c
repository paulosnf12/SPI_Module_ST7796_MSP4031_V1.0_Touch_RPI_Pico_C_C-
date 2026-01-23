#include <stdio.h>
#include <string.h>
#include <time.h> // Necessário para converter timestamp
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "pico/multicore.h"
#include "hardware/rtc.h" // Biblioteca do Relógio Interno
#include "pico/util/datetime.h"

// Includes para o cliente NTP (UDP)
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "wifi_handler.h"

// Configurações Wi-Fi (Pode vir do wifi_handler.h se estiver usando a integração anterior)
#ifndef WIFI_SSID
#define WIFI_SSID "psn"
#define WIFI_PASS "kelvin123"
#endif

#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800 // Segundos entre 1900 e 1970
#define TEST_ITERATIONS 10
#define TIMEZONE_OFFSET -3   // Fuso horário do Brasil (UTC-3)

// Buffer para resposta HTTP
char http_response[1024];

// ----- Módulo HTTP (Seu código original) -----
void create_http_response() {
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: close\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head><meta charset=\"UTF-8\"><title>Página Pico W</title></head>"
        "<body>"
        "<h1>Bem-vindo à página da Pico W!</h1>"
        "<p>Hora atual sincronizada via NTP!</p>"
        "</body>"
        "</html>\r\n");
}

static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
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
    printf("Servidor HTTP rodando na porta 80...\n");
}

// ----- INÍCIO DO MÓDULO NTP (RTC) -----

// Chamado quando recebemos resposta do servidor NTP
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Verifica se a resposta é válida
    if (port == NTP_PORT && p->tot_len == NTP_MSG_LEN && mode == 4 && stratum != 0) {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        
        // Converte bytes para inteiro (Big Endian para Host)
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;

        // Aplica o Fuso Horário (-3h Brasil)
        time_t raw_time = seconds_since_1970 + (TIMEZONE_OFFSET * 3600);
        struct tm *utc = gmtime(&raw_time);

        // Preenche a estrutura de data/hora do Pico
        datetime_t t = {
            .year  = utc->tm_year + 1900,
            .month = utc->tm_mon + 1,
            .day   = utc->tm_mday,
            .dotw  = utc->tm_wday,
            .hour  = utc->tm_hour,
            .min   = utc->tm_min,
            .sec   = utc->tm_sec
        };

        // Configura o Relógio Interno (RTC)
        rtc_init();
        rtc_set_datetime(&t);
        
        printf("[NTP] Hora sincronizada: %02d/%02d/%04d %02d:%02d:%02d\n", 
               t.day, t.month, t.year, t.hour, t.min, t.sec);
    }
    pbuf_free(p);
}

// Envia o pedido para o servidor NTP
static void ntp_request(struct udp_pcb *pcb, const ip_addr_t *addr) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *) p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b; // Configuração padrão NTP
    udp_sendto(pcb, p, addr, NTP_PORT);
    pbuf_free(p);
}

// Resolve o DNS do servidor NTP e inicia o processo
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    struct udp_pcb *pcb = (struct udp_pcb *)arg;
    if (ipaddr) {
        printf("[NTP] Endereço do servidor encontrado: %s\n", ip4addr_ntoa(ipaddr));
        ntp_request(pcb, ipaddr);
    } else {
        printf("[NTP] Falha na resolução DNS\n");
    }
}

// Função para iniciar a sincronização (chamada no loop principal)
void sync_rtc_with_ntp() {
    struct udp_pcb *pcb = udp_new();
    if (!pcb) return;
    
    udp_recv(pcb, ntp_recv, NULL);
    
    ip_addr_t ntp_server_ip;
    int err = dns_gethostbyname(NTP_SERVER, &ntp_server_ip, ntp_dns_found, pcb);
    
    if (err == ERR_OK) {
        ntp_request(pcb, &ntp_server_ip);
    }
}

// ----- FIM DO MÓDULO NTP -----


// Função para rodar o Wi-Fi em paralelo no Core 1
void wifi_task() {
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

    // Loop de Conexão
    while (true) {
        printf("Tentando conectar ao Wi-Fi...\n");
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 5000) == 0) {
            printf("Wi-Fi conectado!\n");
            
            uint8_t *ip = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
            printf("Endereço IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

            start_http_server(); 
            
            // --- NOVO: SINCRONIZAR HORA ---
            printf("Iniciando sincronização NTP...\n");
            sync_rtc_with_ntp();
            // -----------------------------

            break;
        }

        printf("Falha na conexão, tentando novamente...\n");
        sleep_ms(2000); 
    }

    // Loop Principal (Manutenção e Print)
    while (true) {
        cyw43_arch_poll(); // Importante para receber o pacote UDP do NTP
        
        static uint64_t last_print_time = 0;
        uint64_t now = time_us_64();

        // Print a cada 1 segundo (1.000.000 us)
        if (now - last_print_time >= 1000000) { 
            
            // Lê o RTC Interno
            datetime_t t;
            if (rtc_get_datetime(&t)) {
                printf("[RTC] %02d/%02d/%04d %02d:%02d:%02d\n", t.day, t.month, t.year, t.hour, t.min, t.sec);
                
                // FUTURO: Aqui você chamará a função para atualizar o LVGL
                // Exemplo: gui_update_clock(t.hour, t.min);
            } else {
                printf("[RTC] Relógio não inicializado ainda (aguardando NTP)...\n");
            }
            
            last_print_time = now;
        }

        sleep_ms(10); 
    }
}