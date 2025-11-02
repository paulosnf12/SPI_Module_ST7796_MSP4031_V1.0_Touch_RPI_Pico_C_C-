/**
 * @file main.c
 * @brief LVGL demo with ST7789 LCD PIO (Programmable I/O) driver.
 *
 * LVGL v9.4.0 demo program interfacing with the ST7789 LCD display
 * controller using the PIO peripheral. LVGL screen was created using
 * Eez Studio and is designed to work with the ST7789 2 inches display.
 *
 * @date 2025-25-10
 * @copyright (c) 2025 Hardware Innovation Technologies. All rights reserved.
 * License: MIT License (see LICENSE file for details)
 */

#include <stdio.h> // For printf

#include "pico/stdlib.h" // For stdio_init_all, sleep_us, sleep_ms
#include "hardware/gpio.h" // For gpio functions
#include "pico/cyw43_arch.h" // For Wi-Fi functions
#include "st7789_lcd_pio.h" // For ST7789 LCD PIO functions
#include "lvgl.h" // For LVGL functions
#include "pico/multicore.h" // For crictical sections
#include "ui/ui.h" // For EEZ studio LVGL generated UI functions
#include "ui_animations.h"
#include "ui_animations_gen.h"
#include "screens/screen_animations_gen.h"
#include "components/buttons/button_normal_gen.h"
#include "components/buttons/button_show_numpad_gen.h"
#include "components/list/list_gen.h"

#include "hardware/i2c.h"
#include "aht10.h"

#include "ft6336u.h"

#define BUTTON_A_PIN 5 // Button A pin from the schematic
#define BUTTON_B_PIN 6 // Button B pin from the schematic
#define DEBOUNCE_DELAY_MS 20 // Debounce delay in milliseconds

// Last known states and last debounce times
bool last_state_a = true;
bool last_state_b = true;
absolute_time_t last_debounce_time_a;
absolute_time_t last_debounce_time_b;

#define SERIAL_CLK_DIV 1.f // Serial clock divider for PIO, adjust as needed

static critical_section_t crit_sec = {0}; //< Synchronization for safe time reading
static lv_display_t * lcd_disp = NULL; //< Pointer to the LVGL display object

#define DEMO_MODE 1

/**
 * Retrieves the number of milliseconds elapsed since the system booted.
 * NEEDED BY LVGL FOR TICK HANDLING.
 * 
 * @return The number of milliseconds since the system started.
 */
uint32_t get_milliseconds_since_boot()
{
    critical_section_enter_blocking(&crit_sec);
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    critical_section_exit(&crit_sec);
    return ms;
}

/**
 * @brief Callback function to flush buffered data.
 * NEEDED BY LVGL FOR DISPLAY FLUSHING.
 * 
 * @return int Returns 0 on success, or a negative value on error.
 */
void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    lcd_set_window(pio, sm, area->x1, area->x2, area->y1, area->y2);
    /* The most simple case (also the slowest) to send all rendered pixels to the
     * screen one-by-one.  `put_px` is just an example.  It needs to be implemented by you. */
    uint16_t * buf16 = (uint16_t *)px_map; /* Let's say it's a 16 bit (RGB565) display */
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //put_px(x, y, *buf16);
            st7789_lcd_put(pio, sm, *buf16 >> 8);
            st7789_lcd_put(pio, sm, *buf16 & 0xff);
            buf16++;
        }
    }

    /* IMPORTANT!!!
     * Inform LVGL that flushing is complete so buffer can be modified again. */
    lv_display_flush_ready(display);
}

/**
 * @brief Initialize the LVGL port display.
 * NEEDED BY LVGL FOR DISPLAY INITIALIZATION.
 * 
 * This function sets up the display driver and configures the necessary hardware
 * and software resources required for LVGL to render graphics on the target display.
 * It should be called during system initialization before any LVGL drawing operations.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
void lv_port_display_init(void)
{
    lcd_init(pio, sm, st7789_init_seq); // initialize the LCD before display driver

    /* Create the LVGL display object and the ST7789 LCD display driver */
    lcd_disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    //lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);   // jrfo - isso não tá funcionando 

    uint32_t buf_size = SCREEN_WIDTH * SCREEN_HEIGHT / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    buf1 = (uint8_t*)lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    // buf2 = (uint8_t*)lv_malloc(buf_size);
    // if(buf2 == NULL) {
    //     LV_LOG_ERROR("display buffer malloc failed");
    //     lv_free(buf1);
    //     return;
    // }
    lv_display_set_buffers(lcd_disp, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

// Function to get the current screen ID
enum ScreensEnum get_current_screen_id(){   
    if (objects.main && lv_obj_get_screen(objects.main) == lv_scr_act()) {
        return SCREEN_ID_MAIN;
    } 
    return SCREEN_ID_MAIN; // Default to main screen if none matches
}

enum scaleGraph {
    SCALE_MINUTE = 1,
    SCALE_HOUR = 2,
    SCALE_DAY = 3,
};

#include <float.h>  // for FLT_MAX and FLT_MIN

float array_max(const float *arr, size_t len) {
    if (len == 0) return -FLT_MAX; // or handle error
    float max_val = arr[0];
    for (size_t i = 1; i < len; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}

float array_min(const float *arr, size_t len) {
    if (len == 0) return FLT_MAX; // or handle error
    float min_val = arr[0];
    for (size_t i = 1; i < len; i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
    }
    return min_val;
}

float array_avg(const float *arr, size_t len) {
    if (len == 0) return 0.0f;  // handle empty array case
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / (float)len;
}

#define CHART_POINT_COUNT 48

static void touch_driver_read(lv_indev_t * indev, lv_indev_data_t * data) {
    static touch_point_t points[2];
    
    uint8_t touch_count = ft6336u_read_touch_points(points);

    if (touch_count > 0) {
        // Se houver pelo menos um toque, reportamos o primeiro
        data->state = LV_INDEV_STATE_PR; // Pressionado

        // --- A PARTE MAIS IMPORTANTE: CALIBRAÇÃO ---
        // Mapeie as coordenadas lidas para a resolução da tela (320x480)
        // Você vai precisar ajustar esta parte!
        data->point.x = 480 - points[0].x;
        data->point.y = points[0].y; 

        printf("Toque em X: %d, Y: %d\n", data->point.x, data->point.y);

    } else {
        // Nenhum toque detectado
        data->state = LV_INDEV_STATE_REL; // Solto
    }
}

lv_obj_t * obj_tela;
/*
// Esta função será o seu callback de evento para os botões numéricos e de texto.
void button_click_event(lv_event_t * e) {
    // Obter o objeto que disparou o evento (o botão pressionado)
    lv_obj_t * btn = lv_event_get_target(e);
    if (btn == NULL) return;

    // Obter o objeto do label dentro do botão (geralmente é o primeiro filho)
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    if (label == NULL) return;

    // Obter o texto do label do botão
    const char * txt = lv_label_get_text(label);

    // Adicionar o texto do botão à sua área de texto (textarea)
    // Supondo que o nome do seu textarea no código C seja 'ui_display'
    lv_textarea_add_text(display_tela, txt);
}

// Uma função separada para o botão "ENTER" para lógica diferente
void enter_button_event(lv_event_t * e) {
    // Aqui você pode adicionar a lógica para o que o botão ENTER deve fazer.
    // Por exemplo, você pode ler o texto completo do textarea:
    const char * full_text = lv_textarea_get_text(display_tela);

    // Ou limpar o textarea
    // lv_textarea_set_text(ui_display, "");

    // Por enquanto, vamos apenas adicionar uma nova linha como exemplo
    lv_textarea_add_text(display_tela, "\n"); 
}
*/

/**
 * @brief Entry point of the program.
 *
 * This function serves as the main entry point for program execution.
 * It initializes necessary resources, executes the core logic, and
 * returns an exit status to the operating system.
 */
int main() {
    stdio_init_all(); // Initialize standard I/O for debugging

    sleep_ms(2000); // Wait for 2 seconds to allow time for USB serial connection
    
    // Initialize Button A and Button B pins
    gpio_init(BUTTON_A_PIN); // Initialize Button A pin
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Set as input
    gpio_pull_up(BUTTON_A_PIN);  // Enable pull-up resistor
    gpio_init(BUTTON_B_PIN); // Initialize Button B pin
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Set as input
    gpio_pull_up(BUTTON_B_PIN); // Enable pull-up resistor
    // Initialize button debounce timers
    last_debounce_time_a = get_absolute_time(); // Initialize last debounce time for Button A
    last_debounce_time_b = get_absolute_time(); // Initialize last debounce time for Button B

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    // Initialize the PIO state machine for ST7789 LCD
    uint offset = pio_add_program(pio, &st7789_lcd_program); // Add the ST7789 LCD PIO program to the PIO state machine
    st7789_lcd_program_init(pio, sm, offset, PIN_DIN, PIN_CLK, SERIAL_CLK_DIV); // Initialize the PIO state machine for ST7789 LCD

    // Initialize GPIO pins for ST7789 LCD
    gpio_init(PIN_CS); // Chip Select pin
    gpio_init(PIN_DC); // Data/Command pin
    gpio_init(PIN_RESET); // Reset pin
    gpio_init(PIN_BL); // Backlight pin
    gpio_set_dir(PIN_CS, GPIO_OUT); // Set Chip Select pin as output
    gpio_set_dir(PIN_DC, GPIO_OUT); // Set Data/Command pin as output
    gpio_set_dir(PIN_RESET, GPIO_OUT); // Set Reset pin as output
    gpio_set_dir(PIN_BL, GPIO_OUT); // Set Backlight pin as output

    // Set initial states for GPIO pins
    gpio_put(PIN_CS, 1); // Set Chip Select pin high
    gpio_put(PIN_RESET, 1); // Set Reset pin high
    gpio_put(PIN_BL, 1); // Set Backlight pin high (turn on backlight)

    // Initialize LVGL library
    lv_init(); // Initialize LVGL library

    lv_tick_set_cb(get_milliseconds_since_boot); // Set the tick callback for LVGL
    lv_port_display_init(); // Initialize the LVGL port display
    lv_display_set_flush_cb(lcd_disp, my_flush_cb); // Set the flush callback for LVGL display

    printf("Hello, world! ST7789 LCD over PIO!\n"); // Print a message to the console

    
    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa o driver de toque
    if (!ft6336u_init(i2c1)) {
        printf("Falha ao inicializar o driver de toque FT6336U!\n");
        // Pode travar aqui ou tentar continuar sem toque
        while(1);
    }

    lv_indev_t *indev_touchscreen = lv_indev_create();
    lv_indev_set_type(indev_touchscreen, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchscreen, touch_driver_read);

    //ui_init(); // Initialize the UI created with Eez Studio
    
    // Initialize UI screens with path to screens folder
    //ui_screens_init("screens/"); // Point to the screens folder for assets
    ui_animations_init("screens/"); // Point to the screens folder for assets
    //lv_xml_create("screens/screen_animations.xml"); // Load the animations screen from XML
    //ui_init("screens/"); // Initialize the UI
   

    /* The generated ui_screens_init() creates the screen objects but does not
     * automatically load them. Make the main screen active so its children
     * are visible. */
    //lv_screen_load(screen_main);

    //ui_init(); // Initialize the UI
    
    obj_tela = screen_animations_create();
    lv_screen_load(obj_tela);

    while(1) {

    
        // define absolute time variable and last button pressed time variable
        absolute_time_t now = get_absolute_time(), last_btn_pressed_time;
        

        enum scaleGraph scale_graph = SCALE_MINUTE; // Scale graph to fit temperature and humidity values 0=minute, 1=hour, 2=day
        static int buffer_index = 0, percentage_to_30min=0;
        static absolute_time_t last_sample_time = 0, last_sample_time2 = 0;
        
        #ifdef DEMO_MODE
            static absolute_time_t last_sample_time3 = 0;
        #endif

        /*
        // --- BLOCO DE TESTE DE SANIDADE ---
        // Adicione este código temporariamente para debug
        static touch_point_t points_teste[2];
        uint8_t count = ft6336u_read_touch_points(points_teste);
        if (count > 0) {
            printf("[TESTE DIRETO] Toque detectado! X=%d, Y=%d\n", points_teste[0].x, points_teste[0].y);
        }
        // --- FIM DO BLOCO DE TESTE ---
        */

        lv_task_handler(); // Handle LVGL tasks
        sleep_ms(10); // Sleep to allow other tasks to run
    }

    // Cleanup
    lv_display_delete(lcd_disp);
    lv_free(buf1);
    lv_free(buf2);
    cyw43_arch_deinit();
    
    return 0;
}