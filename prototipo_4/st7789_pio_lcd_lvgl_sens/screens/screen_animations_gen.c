/**
 * @file screen_animations_gen.c
 * @brief Template source file for LVGL objects
 * * Este arquivo define a tela principal 'screen_animations_#' e gerencia
 * seus componentes visuais (Visor, Infinite Knob, Lista) e suas animações.
 * O foco está em usar GESTURES (swipe) para mostrar/esconder uma lista
 * lateral, limitando a área de detecção do toque para evitar conflitos
 * com outros widgets na tela.
 */

/*********************
 * INCLUDES
 *********************/
#include "screen_animations_gen.h"
#include "../ui_animations.h" // Funções de animação globais.
#include "../components/arc/arc_gen.h" // Componente Arc (comentado/substituído).
#include "../components/visor/visor_gen.h" // Widget de Visor (Display principal).
#include "../components/arc/infinite_knob.h" // Knob infinito (arc infinito).


/***********************
 * STATIC PROTOTYPES
 ***********************/
// Cria a timeline de animação para a abertura da tela (atualmente vazia).
static lv_anim_timeline_t * timeline_screen_open_create(lv_obj_t * obj);
// Callback para liberar a memória das timelines ao deletar a tela.
static void free_timeline_event_cb(lv_event_t * e);
// Callback original do botão de seta (atualmente desabilitado, substituído por swipe).
// static void botao_seta_toggle_cb(lv_event_t * e);


// Handler principal para detecção de gestos (swipe).
static void swipe_event_cb(lv_event_t * e);
// Função executora (exec_cb) da animação: atualiza a posição X do objeto.
static void anim_list_x(lv_obj_t * obj, int32_t v);
// Inicia a animação para deslizar a lista para DENTRO da tela (mostrar).
static void open_list_with_anim(lv_obj_t * list);
// Inicia a animação para deslizar a lista para FORA da tela (esconder).
static void close_list_with_anim(lv_obj_t * list);

/* callback para esconder após animação terminar */
// Garante que o objeto seja setado como HIDDEN apenas após o término da animação de fechamento.
static void hide_list_ready_cb(lv_anim_t * a);

/**********************
 * GLOBAL FUNCTIONS
 **********************/
/**
 * @brief Cria a tela principal, seus componentes e associa os eventos.
 * @return lv_obj_t* Objeto raiz da tela.
 */
lv_obj_t * screen_animations_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    /* ROOT SCREEN OBJECT */
    // Cria o objeto base da tela.
    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "screen_animations_#");
    lv_obj_set_size(lv_obj_0, 480, 320); // Dimensões fixas (W=480, H=320).
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x1a1a1a), 0); // Cor de fundo escura.

    /***************************
     * BOTÃO SETA
     ***************************/
    // Seção do botão de seta original (agora comentado e desabilitado).

    /*
    lv_obj_t * botao_seta =
        button_show_numpad_create(lv_obj_0, seta_esquerda, false, false);
    // ... configurações de posição e tamanho do botão ...
    */

    /***************************
     * LISTA (TOP LAYER)
     ***************************/
    // A lista é criada na camada superior (lv_layer_top) para flutuar sobre todos os outros elementos.
    lv_obj_t * button_list = list_create(lv_layer_top());
    lv_obj_set_name(button_list, "button_list");
    lv_obj_set_size(button_list, 220, 300);
    
    /* Posiciona inicialmente FORA da tela à direita */
    // X=480 (largura total) garante que a lista esteja fora da vista, Y=10.
    lv_obj_set_pos(button_list, 480, 10);

    // Inicialmente a lista está escondida.
    lv_obj_add_flag(button_list, LV_OBJ_FLAG_HIDDEN);

    /***************************
     * VISOR
     ***************************/
    // Cria o componente de visor centralizado.
    lv_obj_t * visor = visor_create(lv_obj_0);
    lv_obj_set_name(visor, "visor");
    lv_obj_set_align(visor, LV_ALIGN_TOP_MID);
    lv_obj_set_pos(visor, 0, 10);
    lv_obj_set_size(visor, 360, 140);

    /***************************
     * ARC
     ***************************/
    // Componente ARC original (comentado).

    /*
    lv_obj_t * arc_widget = arc_create(lv_obj_0);
    // ... configurações ...
    arc_bind_to_visor();
    */


        /***************************
     * INFINITE KNOB
     ***************************/
    // Componente que substituiu o ARC, geralmente um dial ou controle rotativo.
    lv_obj_t * knob_widget = infinite_knob_create(lv_obj_0);
    lv_obj_set_name(knob_widget, "knob_widget");
    lv_obj_set_align(knob_widget, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_x(knob_widget, 30);
    lv_obj_set_y(knob_widget, -10);

    // Associa o Knob ao Visor (Knob controla um valor que é exibido no Visor).
    infinite_knob_bind_to_visor();


    /***************************
     * TIMELINE
     ***************************/
    // Aloca um array para armazenar as Timelines de Animação.
    lv_anim_timeline_t ** at_array =
        lv_malloc(sizeof(lv_anim_timeline_t *) * _SCREEN_ANIMATIONS_TIMELINE_CNT);

    // Cria a timeline de abertura da tela (atualmente vazia).
    at_array[SCREEN_ANIMATIONS_TIMELINE_SCREEN_OPEN] =
        timeline_screen_open_create(lv_obj_0);

    // Armazena o array de timelines nos user data da tela.
    lv_obj_set_user_data(lv_obj_0, at_array);
    // Adiciona um callback para liberar a memória das timelines quando a tela for deletada.
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array);

    /* CALLBACK DO BOTÃO SETA */
    // Botão desativado. O evento de clique não está mais sendo usado para o toggle da lista.
    //lv_obj_add_event_cb(botao_seta, botao_seta_toggle_cb, LV_EVENT_CLICKED, NULL);

    /* CALLBACK DE SWIPE */
    // Associa a função de tratamento de gestos (swipe) ao objeto raiz da tela.
    lv_obj_add_event_cb(lv_obj_0, swipe_event_cb, LV_EVENT_GESTURE, NULL);

    LV_TRACE_OBJ_CREATE("finished");
    return lv_obj_0;
}

/**********************
 * TIMELINE HANDLING
 **********************/
/**
 * @brief Retorna uma timeline específica pelo seu ID.
 */
lv_anim_timeline_t *
screen_animations_get_timeline(lv_obj_t * obj,
                               screen_animations_timeline_t timeline_id)
{
    // Verifica se o ID solicitado é válido.
    if (timeline_id >= _SCREEN_ANIMATIONS_TIMELINE_CNT) {
        LV_LOG_WARN("screen_animations has no timeline with %d ID", timeline_id);
        return NULL;
    }

    // Recupera o array de timelines e retorna a timeline solicitada.
    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

/**
 * @brief Cria uma timeline de animação. Atualmente, apenas cria e retorna um objeto vazio.
 */
static lv_anim_timeline_t * timeline_screen_open_create(lv_obj_t * obj)
{
    return lv_anim_timeline_create();
}

/**
 * @brief Handler chamado quando a tela é deletada (LV_EVENT_DELETE).
 * Libera a memória alocada para o array de timelines e deleta as timelines.
 */
static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);

    // 1. Proteção: Se o array principal for nulo, sai fora.
    if (at_array == NULL) return;

    for (uint32_t i = 0; i < _SCREEN_ANIMATIONS_TIMELINE_CNT; i++) {
        // 2. Proteção: Só tenta deletar se o ponteiro for válido (não NULL)
        if (at_array[i] != NULL) {
            lv_anim_timeline_delete(at_array[i]);
            at_array[i] = NULL; // Boa prática: anular após deletar
        }
    }

    lv_free(at_array);
}

/**********************
 * CALLBACK DO BOTÃO
 **********************/
/**
 * @brief Toggle da lista usando um botão (Função não utilizada no código atual,
 * mas mantida para referência/alternativa).
 */
static void botao_seta_toggle_cb(lv_event_t * e)
{
    lv_obj_t * list = lv_obj_find_by_name(lv_layer_top(), "button_list");

    // Se estiver escondida, abre. Se estiver visível, fecha.
    if (lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
        open_list_with_anim(list);
    else
        close_list_with_anim(list);
}

/***************************************************
 * ANIMAÇÃO DA LISTA
 ***************************************************/
/**
 * @brief Função Executora de Animação (exec_cb).
 * Define a função que será chamada em cada frame da animação para atualizar a posição X.
 * @param obj Objeto a ser animado (a lista).
 * @param v Novo valor de X.
 */
static void anim_list_x(lv_obj_t * obj, int32_t v)
{
    lv_obj_set_x(obj, v);
}

/**
 * @brief Inicia a animação para deslizar a lista para dentro da tela.
 * O movimento é de X=480 (fora) para X=260 (dentro, no canto superior direito).
 */
static void open_list_with_anim(lv_obj_t * list)
{
    // Remove a flag HIDDEN para que o objeto seja desenhado.
    lv_obj_clear_flag(list, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240); // Duração de 240ms.

    // Path 'ease_out' para uma parada mais suave.
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    lv_anim_set_values(&a,
                       480,         /* posição inicial fora da tela (X=480) */
                       260);        /* posição final dentro (260 é a coordenada X para alinhar à direita) */

    lv_anim_start(&a);
}

/* callback correto para esconder ao finalizar */
/**
 * @brief Callback chamado quando a animação de fechamento termina.
 * Garante que o objeto seja setado como HIDDEN para economizar recursos de renderização.
 */
static void hide_list_ready_cb(lv_anim_t * a)
{
    lv_obj_t * list = lv_anim_get_user_data(a);
    lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief Inicia a animação para deslizar a lista para fora da tela.
 * O movimento é da posição atual (X=260) para X=480 (fora).
 */
static void close_list_with_anim(lv_obj_t * list)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, list);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_list_x);
    lv_anim_set_time(&a, 240);
    // Path 'ease_in' para uma aceleração mais suave ao sair.
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);

    lv_anim_set_values(&a,
                       lv_obj_get_x(list),  /* posição inicial (onde a lista está) */
                       480);                /* sai da tela (X=480) */

    /* CORRETO: esconde só quando a animação termina */
    // Usa o callback hide_list_ready_cb para esconder o objeto ao finalizar.
    lv_anim_set_ready_cb(&a, hide_list_ready_cb);
    lv_anim_set_user_data(&a, list);

    lv_anim_start(&a);
}

/***************************************************
 * SWIPE EVENT (GESTURE)
 ***************************************************/
/**
 * @brief Handler de evento de gesto (swipe).
 * * Implementa a lógica para abrir/fechar a lista através de gestos,
 * com uma regra de limitação da área de toque para o gesto de abertura.
 * * @param e Evento de gesto.
 */
static void swipe_event_cb(lv_event_t * e)
{
    // Obtém a direção do gesto.
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    // Encontra a lista.
    lv_obj_t * list = lv_obj_find_by_name(lv_layer_top(), "button_list");

    if (!list) return;

    /* Obter a posição do ponto de entrada (toque) */
    // Pega a coordenada inicial do toque. ESSENCIAL para limitar a área de detecção.
    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);

    /* Limita a detecção de GESTO de ABRIR a lista (LV_DIR_LEFT)
     * para a metade direita da tela (X >= 240, se a largura total for 480).
     */
    if (dir == LV_DIR_LEFT) {
        // CONDIÇÃO INVERTIDA: Agora só abre se o toque inicial for na metade direita (X >= 240).
        // Isso evita que o swipe interfira em widgets na metade esquerda da tela.
        if (p.x >= 240) { 
            if (lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
                open_list_with_anim(list);
        }
    }
    else if (dir == LV_DIR_RIGHT) {
        /*
         * O gesto de fechar (swipe para a direita) deve ser permitido em qualquer lugar
         * que a lista esteja visível, para que o usuário possa "empurrá-la para fora".
         */
        if (!lv_obj_has_flag(list, LV_OBJ_FLAG_HIDDEN))
            close_list_with_anim(list);
    }
}