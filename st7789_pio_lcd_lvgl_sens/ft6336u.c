/**
 * @file ft6336u.c
 * @brief LVGL demo with ST7789 LCD PIO (Programmable I/O) driver.
 *
 * LVGL v9.4.0 demo program interfacing with the ST7789 LCD display
 * controller using the PIO peripheral. LVGL screen was created using
 * Eez Studio and is designed to work with the ST7789 2 inches display.
 *
 * @author Paulo Santos do Nascimento Filho
 * @date 2025-25-10
 * @copyright (c) 2025 Hardware Innovation Technologies. All rights reserved.
 * License: MIT License (see LICENSE file for details)
 */

#include "ft6336u.h"
#include <stdio.h>

// Endereço I2C do chip
#define FT6336U_I2C_ADDRESS     0x38

// Pinos (defina de acordo com sua fiação)
#define I2C_PORT                i2c1
#define TOUCH_RESET_PIN         8

// Registradores (copiados do driver que você encontrou)
#define FT6X36_TD_STAT_REG      0x02
#define FT6X36_P1_XH_REG        0x03
#define FT6X36_P2_XH_REG        0x09
#define FT6X36_PANEL_ID_REG     0xA8
#define FT6X36_CHIPSELECT_REG   0xA3

// --- FUNÇÃO DE "TRADUÇÃO" PARA I2C ---
// O driver antigo usa IIC_Read_byte. Criaremos uma versão para o Pico.
// Retorna 0 em sucesso, -1 em erro.
static int pico_i2c_read_bytes(uint8_t reg, uint8_t *buf, uint8_t len) {
    if (i2c_write_blocking(I2C_PORT, FT6336U_I2C_ADDRESS, &reg, 1, true) != 1) {
        return -1; // Falha ao escrever o endereço do registrador
    }
    if (i2c_read_blocking(I2C_PORT, FT6336U_I2C_ADDRESS, buf, len, false) != len) {
        return -1; // Falha ao ler os dados
    }
    return 0;
}

// --- FUNÇÕES DO DRIVER ADAPTADAS ---

// Função de inicialização adaptada
bool ft6336u_init(i2c_inst_t *i2c_port) {
    // Configura o pino de Reset
    gpio_init(TOUCH_RESET_PIN);
    gpio_set_dir(TOUCH_RESET_PIN, GPIO_OUT);

    // Sequência de reset (igual à do driver encontrado, mas com funções do Pico)
    gpio_put(TOUCH_RESET_PIN, 0);
    sleep_ms(20);
    gpio_put(TOUCH_RESET_PIN, 1);
    sleep_ms(300);

    printf("ft6336u i2c init\r\n");

    uint8_t data_buf;

    // Verifica a comunicação lendo alguns registradores (ótimo para debug)
    if (pico_i2c_read_bytes(FT6X36_PANEL_ID_REG, &data_buf, 1) != 0) {
        printf("ERRO: Nao foi possivel ler o Device ID\r\n");
        return false;
    }
    printf("Touch Device ID: 0x%02x\r\n", data_buf);

    if (pico_i2c_read_bytes(FT6X36_CHIPSELECT_REG, &data_buf, 1) != 0) {
         printf("ERRO: Nao foi possivel ler o Chip ID\r\n");
        return false;
    }
    printf("Touch Chip ID: 0x%02x\r\n", data_buf);

    return true;
}

// Função de leitura adaptada
uint8_t ft6336u_read_touch_points(touch_point_t *points_out) {
    uint8_t point_data[4];
    uint8_t touch_count = 0;

    // 1. Lê o registrador de status para saber quantos toques existem
    if (pico_i2c_read_bytes(FT6X36_TD_STAT_REG, &touch_count, 1) != 0) {
        return 0; // Erro de comunicação
    }
    touch_count &= 0x0F; // Pega apenas os bits relevantes

    if (touch_count == 0 || touch_count > 2) {
        return 0; // Nenhum toque válido
    }

    // 2. Lê os dados do primeiro ponto de toque
    if (pico_i2c_read_bytes(FT6X36_P1_XH_REG, point_data, 4) != 0) {
        return 0; // Erro de comunicação
    }
    
    // 3. Decodifica as coordenadas (lógica copiada do driver que você achou)
    // Note a inversão de X e Y no driver original, vamos manter por enquanto.
    points_out[0].x = ((point_data[2] & 0x0F) << 8) | point_data[3];
    points_out[0].y = ((point_data[0] & 0x0F) << 8) | point_data[1];

    if (touch_count > 1) {
        // Se houver um segundo toque, faz a mesma coisa para o P2
        if (pico_i2c_read_bytes(FT6X36_P2_XH_REG, point_data, 4) != 0) {
            return 1; // Retorna apenas 1 ponto se a leitura do segundo falhar
        }
        points_out[1].x = ((point_data[2] & 0x0F) << 8) | point_data[3];
        points_out[1].y = ((point_data[0] & 0x0F) << 8) | point_data[1];
    }
    
    return touch_count;
}
