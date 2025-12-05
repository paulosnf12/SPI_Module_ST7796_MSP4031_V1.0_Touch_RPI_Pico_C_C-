#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Endereço I2C do MCP4017 conforme Tabela 5-2 do datasheet
// O endereço fixo é '0101111' binário = 0x2F hexadecimal
#define MCP4017_ADDR 0x2F  // 

// Definições dos Pinos I2C (Padrão I2C0 no Pico)
#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

// Função para escrever o valor do Wiper (0-127)
void mcp4017_write_wiper(uint8_t val) {
    // O MCP4017 é 7-bit, então garantimos que o MSB seja 0 (máscara 0x7F)
    // O datasheet informa que o MSb é "don't care", mas limitamos por segurança [cite: 2783]
    uint8_t data = val & 0x7F;

    // Escreve 1 byte no barramento. 
    // O protocolo é: Start -> Address(Write) -> ACK -> DataByte -> ACK -> Stop 
    int ret = i2c_write_blocking(I2C_PORT, MCP4017_ADDR, &data, 1, false);
    
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao escrever no MCP4017\n");
    }
}

// Função para ler o valor atual do Wiper
uint8_t mcp4017_read_wiper() {
    uint8_t data;
    
    // Lê 1 byte do barramento.
    // O protocolo é: Start -> Address(Read) -> ACK -> DataByte -> NACK -> Stop [cite: 2758]
    int ret = i2c_read_blocking(I2C_PORT, MCP4017_ADDR, &data, 1, false);
    
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao ler do MCP4017\n");
        return 0;
    }
    return data;
}

int main() {
    stdio_init_all();

    // Aguarda um pouco para o serial conectar (útil para debug via USB)
    sleep_ms(2000);
    printf("Iniciando MCP4017 Exemplo I2C\n");

    // Inicializa o I2C a 100kHz (Standard Mode) ou 400kHz (Fast Mode)
    // O datasheet suporta até 400kHz [cite: 621]
    i2c_init(I2C_PORT, 400 * 1000);
    
    // Configura os GPIOs para função I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    
    // Ativa pull-ups internos (remova se usar resistores externos de 4.7k)
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    while (1) {
        // Exemplo: Rampa de subida (0 a 127)
        // O dispositivo tem 128 passos (0x00 a 0x7F) 
        printf("Aumentando resistencia...\n");
        for (int i = 0; i <= 127; i++) {
            mcp4017_write_wiper((uint8_t)i);
            
            // Leitura de verificação (opcional)
            uint8_t read_val = mcp4017_read_wiper();
            printf("Wiper Set: %d | Lido: %d\n", i, read_val);
            
            sleep_ms(20); 
        }

        sleep_ms(1000);

        // Exemplo: Rampa de descida (127 a 0)
        printf("Diminuindo resistencia...\n");
        for (int i = 127; i >= 0; i--) {
            mcp4017_write_wiper((uint8_t)i);
            sleep_ms(20);
        }
        
        sleep_ms(1000);
    }

    return 0;
}