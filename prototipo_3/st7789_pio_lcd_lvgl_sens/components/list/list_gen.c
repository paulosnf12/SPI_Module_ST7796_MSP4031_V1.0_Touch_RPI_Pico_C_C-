/**
 * @file list_gen.c
 * @brief Implemneta um componente de lista customizado com display e botões.
 */

/*********************
 * INCLUDES
 *********************/
#include "list_gen.h" // Cabeçalho do componente de lista.
#include "../../ui_animations.h" // Dependência para animações globais.
#include "../visor/visor_gen.h" // Dependência do componente de display.
#include <string.h> // Para manipulação de strings.
#include <stdlib.h> // Para funções utilitárias (ex: atof).

/*********************
 * DEFINES
 *********************/

/**********************
 * TYPEDEFS
 **********************/

/***********************
 * STATIC VARIABLES
 ***********************/
static char last_enter_value[32] = ""; // Armazena o último valor confirmado.

/***********************
 * STATIC PROTOTYPES
 ***********************/
static lv_anim_timeline_t * timeline_list_open_create(lv_obj_t * obj); // Cria a timeline de animação para abrir a lista.
static void free_timeline_event_cb(lv_event_t * e); // Libera memória das timelines ao deletar a lista.

// Funções para manipulação do display e eventos dos botões
static lv_obj_t * list_get_display(lv_obj_t * list); // Retorna o objeto display dentro da lista.
static void list_button_event_cb(lv_event_t * e); // Handler de eventos para os botões da lista.
void list_connect_events(lv_obj_t * list); // Conecta os eventos necessários aos botões e display.

/**********************
 * GLOBAL FUNCTIONS
 **********************/
lv_obj_t * list_create(lv_obj_t * parent)  // Cria o componente de lista.
{
    LV_TRACE_OBJ_CREATE("begin"); // Início do rastreamento de criação.

    lv_obj_t * lv_obj_0 = lv_obj_create(parent); // Cria o objeto base da lista.
    lv_obj_set_name_static(lv_obj_0, "list_#"); // Nome do objeto.
    lv_obj_set_flex_flow(lv_obj_0, LV_FLEX_FLOW_ROW_WRAP); // Layout flexível em linha com quebra.
    lv_obj_set_style_pad_all(lv_obj_0, UNIT_MEDIUM, 0); // Padding interno.
    lv_obj_set_style_flex_main_place(lv_obj_0, LV_FLEX_ALIGN_SPACE_EVENLY, 0); // Espaçamento entre itens.
    lv_obj_set_style_flex_track_place(lv_obj_0, LV_FLEX_ALIGN_SPACE_EVENLY, 0); // Espaçamento entre linhas.
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x0a0a0a), 0); // Cor de fundo.
    lv_obj_set_width(lv_obj_0, lv_pct(100)); // Largura total do pai.
    lv_obj_set_height(lv_obj_0, lv_pct(100)); // Altura total do pai.
    lv_obj_set_style_radius(lv_obj_0, 10, 0); // Bordas arredondadas.
    lv_obj_set_style_border_width(lv_obj_0, 2, 0); // Largura da borda.
    lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e90ff), 0); //  Cor da borda.
    lv_obj_set_style_shadow_width(lv_obj_0, 10, 0); //  Largura da sombra.
    lv_obj_set_style_shadow_color(lv_obj_0, lv_color_hex(0x102030), 0); // Cor da sombra.

    // DISPLAY
    // Cria o display (textarea) dentro da lista.
    // Configurações visuais e funcionais do display.
    lv_obj_t * display = lv_textarea_create(lv_obj_0); // Cria o textarea para o display.
    lv_obj_set_name(display, "display"); // Nome do display.
    lv_obj_set_width(display, lv_pct(95)); // Largura do display.
    lv_obj_set_height(display, lv_pct(15)); //  Altura do display.
    lv_obj_set_style_text_align(display, LV_TEXT_ALIGN_RIGHT, 0); // Alinhamento do texto à direita.
    lv_obj_set_style_text_color(display, lv_color_hex(0xffffff), 0); // Cor do texto.
    lv_obj_set_style_bg_color(display, lv_color_hex(0x001030), 0); // Cor de fundo do display.
    lv_obj_set_style_border_color(display, lv_color_hex(0x1e90ff), 0); // Cor da borda do display.
    lv_obj_set_style_border_width(display, 2, 0); // Largura da borda do display.
    lv_obj_set_style_radius(display, 8, 0); // Bordas arredondadas do display.
    lv_obj_set_style_pad_all(display, UNIT_SMALL, 0); // Padding interno do display.

    /* BOTÕES */
    // Cria 9 botões numerados (0-8) dentro da lista.
    // Configurações visuais e funcionais dos botões.
    // ******Precisa ser otimizado futuramente, com a criação de estilos para os botões ******
    lv_obj_t * btn_0 = button_normal_create(lv_obj_0, "0"); // Cria o botão "0".
    lv_obj_set_name(btn_0, "btn_0"); // Nome do botão.
    lv_obj_set_width(btn_0, lv_pct(30)); // Largura do botão.
    lv_obj_set_style_bg_color(btn_0, lv_color_hex(0x1e1e2e), 0); // Cor de fundo do botão.
    lv_obj_set_style_text_color(btn_0, lv_color_hex(0xffffff), 0); // Cor do texto do botão.
    lv_obj_set_style_bg_grad_color(btn_0, lv_color_hex(0x2a2a3a), 0); // Cor do gradiente do botão.
    lv_obj_set_style_border_color(btn_0, lv_color_hex(0x1e90ff), 0); // Cor da borda do botão.

    // Os codigos abaixo criam os botões de 1 a 8 com configurações similares.

    lv_obj_t * btn_1 = button_normal_create(lv_obj_0, "1");
    lv_obj_set_name(btn_1, "btn_1");
    lv_obj_set_width(btn_1, lv_pct(30));
    lv_obj_set_style_bg_color(btn_1, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_1, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_1, lv_color_hex(0x2a2a3a), 0);
    lv_obj_set_style_border_color(btn_1, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_2 = button_normal_create(lv_obj_0, "2");
    lv_obj_set_name(btn_2, "btn_2");
    lv_obj_set_width(btn_2, lv_pct(30));
    lv_obj_set_style_bg_color(btn_2, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_2, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_2, lv_color_hex(0x2a2a3a), 0);
    lv_obj_set_style_border_color(btn_2, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_3 = button_normal_create(lv_obj_0, "3");
    lv_obj_set_name(btn_3, "btn_3");
    lv_obj_set_width(btn_3, lv_pct(30));
    lv_obj_set_style_bg_color(btn_3, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_3, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_3, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_4 = button_normal_create(lv_obj_0, "4");
    lv_obj_set_name(btn_4, "btn_4");
    lv_obj_set_width(btn_4, lv_pct(30));
    lv_obj_set_style_bg_color(btn_4, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_4, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_4, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_5 = button_normal_create(lv_obj_0, "5");
    lv_obj_set_name(btn_5, "btn_5");
    lv_obj_set_width(btn_5, lv_pct(30));
    lv_obj_set_style_bg_color(btn_5, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_5, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_5, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_6 = button_normal_create(lv_obj_0, "6");
    lv_obj_set_name(btn_6, "btn_6");
    lv_obj_set_width(btn_6, lv_pct(30));
    lv_obj_set_style_bg_color(btn_6, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_6, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_6, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_7 = button_normal_create(lv_obj_0, "7");
    lv_obj_set_name(btn_7, "btn_7");
    lv_obj_set_width(btn_7, lv_pct(30));
    lv_obj_set_style_bg_color(btn_7, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_7, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_7, lv_color_hex(0x1e90ff), 0);

    lv_obj_t * btn_8 = button_normal_create(lv_obj_0, "8");
    lv_obj_set_name(btn_8, "btn_8");
    lv_obj_set_width(btn_8, lv_pct(30));
    lv_obj_set_style_bg_color(btn_8, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_8, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_8, lv_color_hex(0x1e90ff), 0);

    // Tecla APAGAR (BackSpace)
    // Cria o botão de backspace com configurações visuais.
    lv_obj_t * btn_backspace = button_normal_create(lv_obj_0, LV_SYMBOL_BACKSPACE); // Cria o botão BackSpace.
    lv_obj_set_name(btn_backspace, "btn_backspace"); // Nome do botão.
    lv_obj_set_width(btn_backspace, lv_pct(30)); // Largura do botão.
    lv_obj_set_style_bg_color(btn_backspace, lv_color_hex(0x0a84ff), 0); // Cor de fundo do botão.
    lv_obj_set_style_text_color(btn_backspace, lv_color_hex(0xffffff), 0); // Cor do texto do botão.
    lv_obj_set_style_border_color(btn_backspace, lv_color_hex(0xffffff), 0); // Cor da borda do botão.

    // Tecla 9
    lv_obj_t * btn_9 = button_normal_create(lv_obj_0, "9"); 
    lv_obj_set_name(btn_9, "btn_9");
    lv_obj_set_width(btn_9, lv_pct(30));
    lv_obj_set_style_bg_color(btn_9, lv_color_hex(0x1e1e2e), 0);
    lv_obj_set_style_text_color(btn_9, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_9, lv_color_hex(0x1e90ff), 0);

    // Tecla MENOS
    lv_obj_t * btn_dbm = button_normal_create(lv_obj_0, "-");
    lv_obj_set_name(btn_dbm, "btn_menos");
    lv_obj_set_width(btn_dbm, lv_pct(30));
    lv_obj_set_style_bg_color(btn_dbm, lv_color_hex(0x0a84ff), 0);
    lv_obj_set_style_text_color(btn_dbm, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(btn_dbm, lv_color_hex(0xffffff), 0);


    // Tecla ENTER
    lv_obj_t * btn_enter = button_normal_create(lv_obj_0, "ENTER");
    lv_obj_set_name(btn_enter, "btn_enter");
    lv_obj_set_width(btn_enter, lv_pct(65));
    lv_obj_set_style_bg_color(btn_enter, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_text_color(btn_enter, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(btn_enter, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_grad_color(btn_enter, lv_color_hex(0x62a8ff), 0);
    lv_obj_set_style_radius(btn_enter, 8, 0);

    lv_obj_t * btn_point = button_normal_create(lv_obj_0, ".");
    lv_obj_set_name(btn_point, "btn_point");
    lv_obj_set_width(btn_point, lv_pct(30));
    lv_obj_set_style_bg_color(btn_point, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_color(btn_point, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(btn_point, lv_color_hex(0x1e90ff), 0);
    lv_obj_set_style_radius(btn_point, 8, 0);

    /* create animation timeline(s) */
    // Cria as timelines de animação e armazena como dados do usuário.
    // Cada timeline é acessível via um ID enumerado.
    lv_anim_timeline_t ** at_array = lv_malloc(sizeof(lv_anim_timeline_t *) * _LIST_TIMELINE_CNT); // Aloca memória para as timelines.
    at_array[LIST_TIMELINE_LIST_OPEN] = timeline_list_open_create(lv_obj_0); // Cria a timeline de abertura da lista.
    lv_obj_set_user_data(lv_obj_0, at_array); // Armazena o array de timelines nos dados do usuário.
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array); // Adiciona callback para liberar memória ao deletar.

    // Conecta eventos dos botões
    list_connect_events(lv_obj_0); 

    // Finaliza a criação do objeto lista.
    LV_TRACE_OBJ_CREATE("finished"); // Fim do rastreamento de criação.
    return lv_obj_0; // Retorna o objeto lista criado.
}

// Retorna uma timeline específica pelo seu ID.
lv_anim_timeline_t * list_get_timeline(lv_obj_t * obj, list_timeline_t timeline_id) // Obtém a timeline de animação pelo ID.
{
    if (timeline_id >= _LIST_TIMELINE_CNT) // Verifica se o ID é válido.
    {
        LV_LOG_WARN("list has no timeline with %d ID", timeline_id); // Log de aviso se inválido.
        return NULL; // Retorna NULL se inválido.
    }
    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj); // Recupera o array de timelines.
    return at_array[timeline_id]; // Retorna a timeline solicitada.
}

/****************************************************
 *FUNÇÕES PARA TXT DO DISPLAY 
 ****************************************************/
// Retorna o objeto display dentro da lista.
// Parâmetros:
// - list: Ponteiro para o objeto lista.
// Retorna:
// - Ponteiro para o objeto display (textarea) dentro da lista.
static lv_obj_t * list_get_display(lv_obj_t * list) // Obtém o objeto display da lista.
{
    return lv_obj_get_child_by_name(list, "display"); // Retorna o filho com nome "display".
}

// Handler de eventos para os botões da lista.
// Parâmetros:
// - e: Ponteiro para o evento LVGL.

static void list_button_event_cb(lv_event_t * e)  // Handler de eventos para os botões da lista.
{
    lv_obj_t * btn = lv_event_get_target(e); // Obtém o botão que disparou o evento.
    lv_obj_t * list = lv_obj_get_parent(btn); //  Obtém o objeto lista pai do botão.
    lv_obj_t * display = list_get_display(list); // Obtém o objeto display dentro da lista.

    if (display == NULL) return; // Verifica se o display é válido.

    lv_obj_t * label = lv_obj_get_child(btn, 0); // Obtém o rótulo (label) do botão.
    if(label == NULL) return; // Verifica se o rótulo é válido.

    const char * btn_txt = lv_label_get_text(label); // Obtém o texto do rótulo do botão.

    // BackSpace
    if (strcmp(btn_txt, LV_SYMBOL_BACKSPACE) == 0)  // Verifica se o botão é BackSpace.
    {
        lv_textarea_delete_char(display); // Remove o último caractere do display.
    } 
    // ENTER
    else if (strcmp(btn_txt, "ENTER") == 0)  // Verifica se o botão é ENTER.
    {
        const char * current_text = lv_textarea_get_text(display); // Obtém o texto atual do display.
        strncpy(last_enter_value, current_text, sizeof(last_enter_value)); // Armazena o texto em last_enter_value.
        last_enter_value[sizeof(last_enter_value)-1] = '\0'; // Garante terminação nula.

        // Atualiza visor
        // Chama funções do visor para atualizar o valor.

        visor_set_value(last_enter_value); // Atualiza o visor com o valor digitado.
        float v = atof(last_enter_value); // Converte o texto para float.
        visor_manual_enter(v); // Chama a função de entrada manual do visor com o valor convertido.

        lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN); // Esconde a lista após o ENTER.
    } 
    // menos sinal
    else if (strcmp(btn_txt, "-") == 0) // Verifica se o botão é o sinal de menos.
    {
        lv_textarea_add_text(display, btn_txt); // Adiciona o sinal de menos ao display.
    } 
    // Ponto decimal
    else if (strcmp(btn_txt, ".") == 0)  // Verifica se o botão é o ponto decimal.
    {
        lv_textarea_add_text(display, btn_txt); // Adiciona o ponto decimal ao display.
    } 
    // Números e ponto
    else 
    {
        lv_textarea_add_text(display, btn_txt); // Adiciona o texto do botão ao display.
    }
}

// Conecta os eventos necessários aos botões e display.
// Parâmetros:
// - list: Ponteiro para o objeto lista.
// Retorna: Nenhum.

void list_connect_events(lv_obj_t * list) // Conecta eventos aos botões da lista.
{
    // Nomes dos botões na lista.
    // Usado para localizar e conectar os eventos.
    // Inclui botões numéricos, ponto, backspace, menos e enter.
    // Adiciona o callback de evento para cada botão.
    // Todos os botões usam o mesmo callback: list_button_event_cb.
    // Isso simplifica o gerenciamento de eventos.
    // O callback lida com a lógica específica com base no texto do botão.
    // Isso permite fácil expansão futura (ex: adicionar mais botões).
    const char * names[] = {
        "btn_0","btn_1","btn_2","btn_3","btn_4",
        "btn_5","btn_6","btn_7","btn_8","btn_9",
        "btn_point", "btn_backspace", "btn_menos", "btn_enter"
    };

    // Loop para conectar eventos a cada botão.
    // Itera sobre o array de nomes dos botões.
    // Adiciona o callback de evento para cada botão encontrado.
    // Isso garante que todos os botões respondam aos cliques.
    // O callback lida com a lógica de entrada no display.
    // Facilita a manutenção e expansão do componente de lista.
    for(int i = 0; i < sizeof(names)/sizeof(names[0]); i++) // Itera sobre os nomes dos botões.
    {
        lv_obj_t * btn = lv_obj_get_child_by_name(list, names[i]); // Obtém o botão pelo nome.
        if(btn) // Verifica se o botão existe.
        {
            lv_obj_add_event_cb(btn, list_button_event_cb, LV_EVENT_CLICKED, NULL); // Adiciona o callback de evento ao botão.
        }
    }
}

/**********************
 * STATIC FUNCTIONS
 **********************/

 // Callback de execução personalizada para animações de inteiros.
 // Parâmetros:
    // - a: Ponteiro para a estrutura de animação LVGL.
    // - v: Valor inteiro atual da animação.
    // Descrição:
    // Esta função é chamada durante a execução de uma animação de inteiros.
    // Ela extrai a propriedade de estilo e o seletor dos dados do usuário da animação.
    // Em seguida, define a propriedade de estilo local do objeto animado com o valor atual.
static void int_anim_exec_cb(lv_anim_t * a, int32_t v) // Callback de execução personalizada para animações de inteiros.
{
    uint32_t data = (lv_uintptr_t)lv_anim_get_user_data(a); // Recupera os dados do usuário da animação.
    lv_style_prop_t prop = data >> 24; //  Extrai a propriedade de estilo.
    lv_style_selector_t selector = data & 0x00ffffff; // Extrai o seletor de estilo.
    lv_style_value_t style_value; // Declara a variável de valor de estilo.
    style_value.num = v; // Define o valor numérico do estilo.
    lv_obj_set_local_style_prop(a->var, prop, style_value, selector); // Define a propriedade de estilo local do objeto.
}

// Cria a timeline de animação para abrir a lista.
// Parâmetros:
// - obj: Ponteiro para o objeto lista.
// Retorna:
// - Ponteiro para a timeline de animação criada.
// Descrição:
// Esta função cria uma timeline de animação que anima a opacidade e a posição X do objeto lista.
// A animação faz com que a lista apareça suavemente na tela.

static lv_anim_timeline_t * timeline_list_open_create(lv_obj_t * obj)  // Cria a timeline de animação para abrir a lista.
{
    lv_anim_timeline_t * at = lv_anim_timeline_create(); // Cria uma nova timeline de animação.
    lv_anim_timeline_t * at_to_merge = NULL; // Declara uma variável para possíveis merges futuros.
    lv_anim_t a; //  Declara a estrutura de animação.
    uint32_t selector_and_prop; // Variável para armazenar seletor e propriedade.

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0; // Configura a animação de opacidade.
    lv_anim_init(&a); // Inicializa a estrutura de animação.
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb); // Define o callback de execução personalizado.
    lv_anim_set_var(&a, obj); // Define o objeto a ser animado.
    lv_anim_set_values(&a, 0, 255); // Define os valores inicial e final da animação.
    lv_anim_set_duration(&a, 300); // Define a duração da animação.
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop)); // Define os dados do usuário da animação.
    lv_anim_timeline_add(at, 0, &a); // Adiciona a animação à timeline.
    lv_anim_set_early_apply(&a, true); // Define para aplicar cedo.

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0; // Configura a animação de posição X.
    lv_anim_init(&a); // Inicializa a estrutura de animação.
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb); // Define o callback de execução personalizado.
    lv_anim_set_var(&a, obj); // Define o objeto a ser animado.
    lv_anim_set_values(&a, 60, 0); // Define os valores inicial e final da animação.
    lv_anim_set_duration(&a, 300); // Define a duração da animação.
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop)); // Define os dados do usuário da animação.
    lv_anim_timeline_add(at, 0, &a); // Adiciona a animação à timeline.
    lv_anim_set_early_apply(&a, true); // Define para aplicar cedo.

    return at; // Retorna a timeline criada.
}
// Libera memória das timelines ao deletar a lista.
// Parâmetros:
// - e: Ponteiro para o evento LVGL.
// Descrição:
// Esta função é chamada quando o objeto lista é deletado.
// Ela recupera o array de timelines armazenado nos dados do usuário,
// deleta cada timeline e libera a memória alocada para o array.

static void free_timeline_event_cb(lv_event_t * e) // Libera memória das timelines ao deletar a lista.
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e); // Recupera o array de timelines dos dados do usuário.
    uint32_t i; // Declara a variável de iteração.
    for(i = 0; i < _LIST_TIMELINE_CNT; i++)  // Itera sobre cada timeline.
    {
        lv_anim_timeline_delete(at_array[i]); // Deleta a timeline.
    }
    lv_free(at_array); // Libera a memória alocada para o array de timelines.
}
