/**
 * @file ft6336u.h
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

#ifndef FT6336U_H
#define FT6336U_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Estrutura para guardar as coordenadas de um ponto de toque
typedef struct {
    uint16_t x;
    uint16_t y;
} touch_point_t;

// Função de inicialização do driver
bool ft6336u_init(i2c_inst_t *i2c_port);

// Função que lê os pontos de toque
// Retorna o número de pontos detectados (0, 1, ou 2)
uint8_t ft6336u_read_touch_points(touch_point_t *points_out);

#endif // FT6336U_H