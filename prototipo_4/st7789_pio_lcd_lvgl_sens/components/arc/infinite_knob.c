/**
 * @file infinite_knob.c
 * @brief Implementa um widget de Knob Rotativo Infinito (Infinite Knob) customizado
 * usando lv_arc, mas com controle manual da lógica de toque e valor.
 */

#include "infinite_knob.h"
#include "../visor/visor_gen.h" // Dependência do componente de display/visor.
#include <stdio.h>               // Para snprintf.
#include <math.h>                // Para sqrtf e atan2f (cálculo trigonométrico).
#include "lvgl.h"                // Biblioteca principal do LVGL.

/*********************
 * CONFIGURAÇÕES
 *********************/
// Constante PI usada nos cálculos trigonométricos (graus para radianos).
#define PI 3.14159265358979323846f

// Limites do Valor
// Os valores mínimo e máximo que o valor interno (current_value) pode atingir.
// Limitado pelo hardware ou aplicação.
#define LIMIT_MAX 17.0f
#define LIMIT_MIN -23.0f

// Sensibilidade (0.02 = ajuste fino)
// Fator de conversão: (diferença angular) * KNOB_SENSITIVITY = (mudança de valor).
// Um valor menor torna o ajuste mais fino.
// Um valor maior torna o ajuste mais rápido.

#define KNOB_SENSITIVITY 0.005f 

// --- CONFIGURAÇÃO DA ÁREA DE TOQUE (EM PIXELS) ---
// O widget tem 140x140. O Raio total é 70.
// Definimos uma área anelar para o toque, ignorando o centro.
// O toque deve estar entre KNOB_RADIUS_INNER e KNOB_RADIUS_OUTER.
// Com isso, evitamos toques acidentais no centro do knob e aumentamos a precisão do toque
#define KNOB_RADIUS_OUTER 70 // Limite externo para a detecção de toque.
#define KNOB_RADIUS_INNER 40 // Limite interno (exclui toques no centro para simular o anel).

// Offset de Rotação Visual
// A função matemática atan2 retorna 0 graus na Direita. 
// O lv_arc está configurado para ter 0 graus no Topo (rotação 270).
// Este offset (90) é usado para alinhar a matemática do toque (0=Direita) com o visual do arco (0=Topo).
#define ARC_ROTATION_OFFSET 90 

/*********************
 * VARIÁVEIS
 *********************/
static float current_value = 0.0f; // Valor numérico atual controlado pelo knob.
static lv_obj_t * arc_obj = NULL;  // Ponteiro para o objeto LVGL (lv_arc).

static float last_angle = 0.0f;    // Último ângulo de toque registrado (para calcular o delta).
static bool is_dragging = false;   // Flag para indicar que um arrasto válido está em andamento.

/*********************
 * PROTOTIPOS
 *********************/
static void visor_enter_cb(float v);                // Callback chamado pelo Visor para atualizar o valor.
static void arc_event_cb(lv_event_t * e);           // Handler principal de eventos de toque/arrasto.
static float calculate_touch_angle(lv_obj_t * obj, lv_point_t * p); // Calcula o ângulo polar do toque.
static bool is_touch_in_ring(lv_obj_t * obj, lv_point_t * p);       // Verifica se o toque está na área anelar.

/************** CREATE *************/
/**
 * @brief Cria e configura o objeto 'Infinite Knob' no LVGL.
 * @param parent Objeto pai do Knob.
 * @return lv_obj_t* Ponteiro para o objeto Arc criado.
 */
lv_obj_t * infinite_knob_create(lv_obj_t * parent) // Cria o Infinite Knob
{
    arc_obj = lv_arc_create(parent); // Cria o objeto lv_arc que servirá como base para o knob.
    lv_obj_set_name(arc_obj, "infinite_knob"); // Nome do objeto para identificação.

    lv_obj_set_size(arc_obj, 140, 140); // Tamanho fixo do knob (140x140 pixels).
    lv_obj_center(arc_obj); // Centraliza o objeto no pai.

    // Configuração Visual: Faz o fundo do arco cobrir 360 graus.
    lv_arc_set_bg_angles(arc_obj, 0, 360); 
  
    lv_arc_set_range(arc_obj, 0, 360);
    // Rotação: Define 0 graus no Topo (12h).
    lv_arc_set_rotation(arc_obj, 270); 

    // Estilo: aumenta a área visual do anel, facilitando o toque.
    lv_obj_set_style_arc_width(arc_obj, 25, LV_PART_MAIN);      // Fundo mais grosso
    lv_obj_set_style_arc_width(arc_obj, 25, LV_PART_INDICATOR); // Frente (indicador) mais grossa
    
    // Removemos a interação padrão do knob (o arrasto) para controlarmos manualmente.
    lv_obj_clear_flag(arc_obj, LV_OBJ_FLAG_CLICKABLE); 
    // Reativamos a flag CLICKABLE para que ele receba os eventos de toque.
    lv_obj_add_flag(arc_obj, LV_OBJ_FLAG_CLICKABLE);

    // Associa o handler customizado para processar os eventos de toque.
    lv_obj_add_event_cb(arc_obj, arc_event_cb, LV_EVENT_ALL, NULL);

    // Valor inicial do indicador visual (posição 12h, 0 graus).
    lv_arc_set_value(arc_obj, 0); // Inicia com o indicador na posição 0.

    return arc_obj;
}

/**
 * @brief Associa a função de callback do Visor para sincronização de valores.
 */
// O Infinite Knob notificará o Visor quando o valor mudar,
void infinite_knob_bind_to_visor(void)
{
    visor_set_enter_callback(visor_enter_cb);
}

/************** LÓGICA ************/

/**
 * @brief Atualiza o valor interno após uma confirmação externa (ex: Visor/Numpad).
 * @param v Novo valor a ser setado.
 */

 // Callback chamado pelo Visor para atualizar o valor do Knob.
static void visor_enter_cb(float v) 
{
    current_value = v; // Atualiza o valor interno do Knob.
    // Garante que o novo valor respeite os limites MIN/MAX.
    if(current_value > LIMIT_MAX) current_value = LIMIT_MAX; // Clamp superior
    if(current_value < LIMIT_MIN) current_value = LIMIT_MIN; // Clamp inferior

    // Atualiza o display do Visor com o novo valor formatado.
    char buf[16]; // Buffer temporário para formatação.
    snprintf(buf, sizeof(buf), "%.2f", current_value); // Formata com 2 casas decimais.
    visor_set_value(buf); // Atualiza o Visor.
    
    // Reseta o indicador visual do arco para a posição inicial (0) após a atualização.
    lv_arc_set_value(arc_obj, 0); 
} 

/**
 * @brief Verifica se o ponto de toque está dentro da coroa circular (anel) do knob.
 * @param obj Objeto Arc.
 * @param p Ponto de toque (coordenadas X, Y).
 * @return true Se o toque estiver entre o raio interno e o raio externo.
 */

 // Verifica se o toque está na área anelar do knob.
static bool is_touch_in_ring(lv_obj_t * obj, lv_point_t * p) // Retorna true se o toque estiver na área anelar
{
    lv_area_t a; // Área do objeto.
    lv_obj_get_coords(obj, &a);
    // Calcula o centro (cx, cy) do objeto.
    float cx = a.x1 + (lv_area_get_width(&a) / 2.0f); // Centro X
    float cy = a.y1 + (lv_area_get_height(&a) / 2.0f); // Centro Y

    float dx = p->x - cx; // Diferença X do toque ao centro
    float dy = p->y - cy; // Diferença Y do toque ao centro
    
    // Teorema de Pitágoras para calcular a distância do toque ao centro (raio).
    float dist = sqrtf(dx*dx + dy*dy);

    // Retorna TRUE se a distância estiver dentro da zona de toque (com tolerância externa).
    return (dist >= KNOB_RADIUS_INNER && dist <= (KNOB_RADIUS_OUTER + 10)); 
}

/**
 * @brief Calcula o ângulo do ponto de toque em relação ao centro do objeto.
 * @param obj Objeto Arc.
 * @param p Ponto de toque (coordenadas X, Y).
 * @return float Ângulo em graus (0-360), onde 0 é a Direita (3h).
 */
static float calculate_touch_angle(lv_obj_t * obj, lv_point_t * p) // Calcula o ângulo polar do toque em graus
{
    lv_area_t a; // Área do objeto.
    lv_obj_get_coords(obj, &a); // Obtém as coordenadas do objeto.
    float cx = a.x1 + (lv_area_get_width(&a) / 2.0f); // Centro X
    float cy = a.y1 + (lv_area_get_height(&a) / 2.0f); // Centro Y

    float dx = p->x - cx; // Diferença X do toque ao centro
    float dy = p->y - cy; // Diferença Y do toque ao centro

    // atan2f: Calcula o ângulo no plano, garantindo o quadrante correto.
    float angle_deg = atan2f(dy, dx) * (180.0f / PI);
    // Normaliza o ângulo para o intervalo [0, 360].
    if(angle_deg < 0) angle_deg += 360.0f;

    return angle_deg;
}

/**
 * @brief Handler principal para todos os eventos de toque/arrasto no Arc.
 */
static void arc_event_cb(lv_event_t * e) // Handler de eventos de toque/arrasto
{
    lv_event_code_t code = lv_event_get_code(e); // Obtém o código do evento.
    lv_obj_t * obj = lv_event_get_target(e); // Obtém o objeto alvo do evento.

    // Filtra apenas os eventos de toque (pressionado, arrastando, solto, perda de foco).
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESS_LOST) {
        return;
    }

    // Obtém o ponto de toque atual.
    lv_indev_t * indev = lv_indev_get_act(); // Dispositivo de entrada ativo.
    if(!indev) return; // Segurança: verifica se o dispositivo é válido.
    lv_point_t p; // Ponto de toque.
    lv_indev_get_point(indev, &p); // Obtém as coordenadas do toque.

    // 1. INÍCIO DO TOQUE (LV_EVENT_PRESSED)
    if(code == LV_EVENT_PRESSED) {
        // Se tocar fora da área anelar, desativa o arrasto e ignora o evento.
        if (!is_touch_in_ring(obj, &p)) {
            is_dragging = false; // Desativa o arrasto
            return; 
        }
        
        // Se válido, armazena o ângulo inicial e define is_dragging como true.
        last_angle = calculate_touch_angle(obj, &p); // Armazena o ângulo inicial do toque.
        is_dragging = true; // Ativa o estado de arrasto.
        return;
    }

    // 2. FIM DO TOQUE (LV_EVENT_RELEASED ou LV_EVENT_PRESS_LOST)
    if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        // Finaliza o estado de arrasto.
        is_dragging = false; // Desativa o arrasto
        return;
    }

    // 3. ARRASTO (LV_EVENT_PRESSING)
    if(code == LV_EVENT_PRESSING && is_dragging) {
        // Processa o arrasto apenas se o toque inicial foi válido.

        float angle = calculate_touch_angle(obj, &p); // Calcula o ângulo atual do toque.
        // Calcula a diferença angular desde o último ponto registrado.
        float diff = angle - last_angle;

        // Correção de Wrap-around (Passagem pelo zero)
        // Lógica para detectar e corrigir a rotação quando o toque cruza o ponto 0/360 graus.
        if(diff < -180.0f) diff += 360.0f; // Ex: 350 -> 10 = +20 graus
        if(diff > 180.0f) diff -= 360.0f; // Ex: 10 -> 350 = -20 graus

        last_angle = angle; // Atualiza o último ângulo.

        if(fabs(diff) < 0.2f) return; // Filtro de ruído: ignora movimentos minúsculos.

        // Atualiza Valor Numérico
        // Calcula a mudança de valor: delta angular * sensibilidade.
        float novo_valor = current_value + (diff * KNOB_SENSITIVITY); // Calcula o novo valor.
        
        // Clamp: Restringe o novo valor aos limites definidos.
        if(novo_valor > LIMIT_MAX) novo_valor = LIMIT_MAX; // Limite superior
        if(novo_valor < LIMIT_MIN) novo_valor = LIMIT_MIN; // Limite inferior

        // Se o valor mudou (após o clamp), atualiza o estado e o visor.
        if(novo_valor != current_value) {
            current_value = novo_valor; // Atualiza o valor interno.
            char buf[16]; // Buffer temporário para formatação.
            snprintf(buf, sizeof(buf), "%.2f", current_value); // Formata com 2 casas decimais.
            visor_set_value(buf);  // Atualiza o Visor com o novo valor.
        }

        // --- VISUAL DA BOLINHA ---
        // Converte o ângulo matemático (0=Direita) para o ângulo visual do LVGL (0=Topo) usando o offset.
        float visual_angle = angle + ARC_ROTATION_OFFSET;
        // Normaliza o ângulo visual para o intervalo [0, 360].
        if(visual_angle >= 360.0f) visual_angle -= 360.0f;
        
        // Atualiza a posição do indicador visual do Arc.
        lv_arc_set_value(obj, (int)visual_angle);
    }
}