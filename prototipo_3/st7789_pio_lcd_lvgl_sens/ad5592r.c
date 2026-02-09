/**
 * @file ad5592r.c
 */

#include "ad5592r.h"
#include "hardware/spi.h"
#include <stdio.h>

// Registradores Internos
#define REG_DAC_CONFIG 0x05
#define REG_CONTROL    0x0B
#define REG_RESET      0x0F

void ad5592r_write_reg(uint8_t pointer, uint16_t data) {
    uint8_t msg[2];
    // Formato: [0][Pointer (4 bits)][Data MSB (3 bits)] + [Data LSB (8 bits)]
    uint16_t pacote = ((uint16_t)pointer << 11) | (data & 0x07FF);

    msg[0] = (uint8_t)(pacote >> 8);
    msg[1] = (uint8_t)(pacote & 0xFF);

    gpio_put(PIN_CS_dac, 0);
    spi_write_blocking(SPI_PORT, msg, 2);
    gpio_put(PIN_CS_dac, 1);
}

void ad5592r_set_dac(uint8_t canal, uint16_t valor) {
    // Seta o nível do DAC (0 a 4095)
    // O Bit 15 deve ser '1' para escrita direta no DAC.
    uint8_t msg[2];
    // Formato: [1][Canal (3 bits)][Valor (12 bits)]
    uint16_t pacote = 0x8000 | ((uint16_t)canal << 12) | (valor & 0x0FFF);

    msg[0] = (uint8_t)(pacote >> 8);
    msg[1] = (uint8_t)(pacote & 0xFF);

    gpio_put(PIN_CS_dac, 0);
    spi_write_blocking(SPI_PORT, msg, 2);
    gpio_put(PIN_CS_dac, 1);
}

void ad5592r_init(void) {
    // Inicializa SPI a 1MHz
    spi_init(SPI_PORT, 1000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    // Configura Pinos SPI
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Configura CS e Reset
    gpio_init(PIN_CS_dac);
    gpio_set_dir(PIN_CS_dac, GPIO_OUT);
    gpio_put(PIN_CS_dac, 1);

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    
    // Reset Hardware
    gpio_put(PIN_RST, 0); 
    sleep_ms(20);
    gpio_put(PIN_RST, 1);
    sleep_ms(20);

    // Reset Software
    ad5592r_write_reg(REG_RESET, 0x5AC);
    sleep_ms(10);

    // Configuração de Referência: Interna desligada (usa 5V externo no pino VREF)
    ad5592r_write_reg(REG_CONTROL, 0x0000);

    // Configura o IO3 como saída DAC
    // Bit 3 em '1' ativa o canal IO3
    ad5592r_write_reg(REG_DAC_CONFIG, 0x0008);
}