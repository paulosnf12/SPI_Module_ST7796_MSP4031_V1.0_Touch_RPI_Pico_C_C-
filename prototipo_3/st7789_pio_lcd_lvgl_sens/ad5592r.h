/**
 * @file ad5592r.h
 * @brief Driver para o AD5592R (Configurado como DAC neste contexto)
 */

#ifndef AD5592R_H
#define AD5592R_H

#include <stdint.h>
#include "pico/stdlib.h"

// Definições de Hardware (Pinos)
#define SPI_PORT    spi1
#define PIN_MISO    12
#define PIN_CS_dac  13
#define PIN_SCK     10
#define PIN_MOSI    11
#define PIN_RST     16

// Canal utilizado para saída de tensão (Conforme seu código original)
#define DAC_CHANNEL_IO3 3

// Protótipos das funções
void ad5592r_init(void);
void ad5592r_write_reg(uint8_t pointer, uint16_t data);
void ad5592r_set_dac(uint8_t canal, uint16_t valor);

#endif // AD5592R_H