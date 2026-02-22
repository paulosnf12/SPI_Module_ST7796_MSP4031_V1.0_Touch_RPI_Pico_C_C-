# Controlador de Potência Óptica com Interface Touchscreen Baseado em VOA

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 👥 Autores
- **João Paulo Magalhães Fernandes**
- **Paulo Santos do Nascimento Filho**

---

## 🎯 Função
Este projeto consiste no desenvolvimento de um sistema embarcado para o controle eletrônico e ajuste preciso da atenuação de um sinal óptico. Utilizando um Atenuador Óptico Variável (VOA) integrado a uma interface gráfica em um display *touchscreen*, o sistema substitui controles analógicos e procedimentos manuais. 

O operador interage com um botão rotativo virtual ("knob" infinito) na tela, definindo o nível de potência desejado (com visualização em mW, µW, nW e dBm). O sistema converte essa interação em um valor digital e atua sobre um conversor digital-analógico (DAC), ajustando a tensão do VOA de forma imediata e contínua. 

**Recursos em destaque:**
- **Interface Gráfica (GUI):** Desenvolvida com a biblioteca LVGL (v9.4.0), conta com resposta *touch* de alta precisão.
- **Controle Direto:** Operação atual em malha aberta, com ajuste em tempo real.
- **Sincronização de Relógio via Wi-Fi:** O sistema possui um menu de configurações onde o usuário pode conectar-se a uma rede Wi-Fi para obtenção e sincronização automática da hora via NTP.
- **Arquitetura Expansível:** O hardware e o software já contêm a base estrutural para uma futura evolução operacional em malha fechada (TRL 6).

---

## 🛠 Hardware e Materiais Usados
O protótipo foi construído com os seguintes componentes:
- **Microcontrolador:** Plataforma BitDogLab baseada no **Raspberry Pi Pico W** (RP2040 com módulo Wi-Fi CYW43439).
- **Display:** Módulo LCD TFT de 2 polegadas com controlador **ST7789** (comunicação via interface PIO do RP2040).
- **Touchscreen:** Painel capacitivo com controlador **FT6336U** (comunicação via I²C).
- **DAC (Conversor Digital-Analógico):** **AD5592R** de 12 bits (configurado para usar 5V de referência externa e comunicação via SPI).
- **Atuador Óptico:** Atenuador Óptico Variável (**VOA**).
- *(Em desenvolvimento para versões futuras: Módulo fotodetector associado a um amplificador de transimpedância OPA380 para leitura em malha fechada).*

---

## 🔄 Fluxo de Controle do Código
A arquitetura de software é dividida em processamento de interface, controle de periféricos e rotinas de rede distribuídas nos dois núcleos do RP2040:

1. **Camada de Interface (LVGL):** O usuário interage com a tela ajustando o *knob*. O evento gera um valor em *float* (mW) baseado na rotação e envia os dados para a rotina de controle.
2. **Camada de Processamento:** O valor em *float* passa por uma função de mapeamento (regra de três linear) sendo convertido para um valor de 12 bits (0 a 4095).
3. **Atuação no Hardware:** Esse valor bruto é enviado via SPI para o DAC AD5592R. O DAC converte o dado digital em uma tensão analógica (0V a 5V) através do canal de saída `IO3`.
4. **Variação Óptica:** A tensão atua fisicamente nos terminais do VOA, que atenua de forma proporcional o laser/sinal óptico que passa pela fibra.
5. **Processamento Paralelo (Core 1):** Uma *Task* independente no segundo núcleo do RP2040 (`wifi_task`) gerencia a tentativa de conexão Wi-Fi e as requisições UDP ao servidor NTP (`pool.ntp.org`) para atualizar o RTC de hardware local sem travar os quadros da interface gráfica do Core 0.

---

## 🚀 Instruções para Replicação

### Pré-requisitos
- C/C++ SDK do Raspberry Pi Pico configurado no seu ambiente.
- Biblioteca **LVGL (v9.4.0)**.
- CMake para build do projeto.

### Cabeamento e Montagem Física
1. **Display (ST7789 via PIO):** Conecte os pinos correspondentes definidos para o PIO (CS, DC, RESET, BL).
2. **Touch (FT6336U via I²C1):** 
   - Conecte `SDA` e `SCL` na interface I²C do Pico. O endereço do chip é `0x38`. 
   - Ligue o pino de interrupção (INT) ao GPIO 9 e o de Reset ao GPIO 8.
3. **DAC (AD5592R via SPI):**
   - Alimente o DAC em 5V através de uma fonte estabilizada (Não use o pino 3.3V do Pico).
   - Realize as conexões da interface SPI (MISO, MOSI, SCK e CS) do Pico ao DAC.
4. **VOA:**
   - Conecte a saída `IO3` do DAC a um dos terminais de controle do VOA.

### Compilação e Execução
1. Clone este repositório:
   ```bash
   git clone <inserir_url_do_repositorio>
   cd <nome_do_diretorio>
   ```
2. Crie e acesse o diretório de build:
   ```bash
   mkdir build && cd build
   ```
3. Execute o CMake e compile o projeto:
   ```bash
   cmake ..
   make -j4
   ```
4. Com o Raspberry Pi Pico W conectado via USB (em modo BOOTSEL), arraste o arquivo `.uf2` gerado na pasta `build` para a unidade montada do Pico.
5. Aguarde a inicialização (a tela de inicialização da HWIT aparecerá e em seguida o *dashboard* de controle do VOA).

**Boas práticas operacionais:**
- Não conecte ou desconecte a fibra óptica com o laser ativo.
- Evite exceder a tensão máxima de operação suportada pelo VOA.

---

## 📚 Referências

- ANALOG DEVICES. *AD5592R: 8-Channel, Configurable ADC/DAC with GPIO.* Norwood, 2019. Datasheet.
- ANALOG DEVICES. *Photodiode Amplifiers: Op Amp Solutions.* Norwood, 2014. Application Note.
- HOROWITZ, Paul; HILL, Winfield. *The Art of Electronics.* 3. ed. Cambridge: Cambridge University Press, 2015.
- IEC. *IEC 60825-1: Safety of laser products – Equipment classification and requirements.* Geneva, 2014.
- KEISER, Gerd. *Optical Fiber Communications.* 5. ed. New York: McGraw-Hill Education, 2021.
- NASA. *Technology Readiness Level Definitions.* Washington, 2012.
- OGATA, Katsuhiko. *Engenharia de Controle Moderno.* 5. ed. São Paulo: Pearson Prentice Hall, 2010.
- RAZAVI, Behzad. *Design of Analog CMOS Integrated Circuits.* 2. ed. New York: McGraw-Hill, 2017.
- RASPBERRY PI TRADING LTD. *RP2040 Microcontroller Datasheet.* 2021.
- SIEGMAN, Anthony E. *Lasers.* Sausalito: University Science Books, 1986.
- TEXAS INSTRUMENTS. *Transimpedance Considerations for High-Speed Amplifiers.* Dallas, 2013. Application Report.
- WILSON, J.; HAWKES, J. F. B. *Optoelectronics: An Introduction.* 3. ed. Harlow: Prentice Hall, 1998.

---

## 📄 Licenças de Uso

Este projeto está sob a licença **MIT**. Você é livre para utilizar, modificar e distribuir o código, desde que mantenha os avisos de direitos autorais originais. Veja o arquivo `LICENSE` para mais detalhes.

## Vídeo de Execução do Projeto

https://github.com/user-attachments/assets/f9815e70-19a1-45fe-bee8-40e30a1069f8

