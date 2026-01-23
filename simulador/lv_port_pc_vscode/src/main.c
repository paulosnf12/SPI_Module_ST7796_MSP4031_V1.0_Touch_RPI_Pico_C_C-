/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include <SDL.h>

#include "hal/hal.h"
#include "projeto/ui_animations.h"
#include "projeto/ui_animations_gen.h"
#include "projeto/screens/screen_animations_gen.h"
#include "projeto/components/buttons/button_normal_gen.h"
#include "projeto/components/buttons/button_show_numpad_gen.h"
#include "projeto/components/list/list_gen.h"
#include "projeto/components/arc/arc_gen.h"
#include "projeto/components/visor/visor_gen.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS != LV_OS_FREERTOS

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  sdl_hal_init(480, 320);

  /* Run the default demo */
  /* To try a different demo or example, replace this with one of: */
  //lv_demo_benchmark();
  //lv_demo_stress();
  /* - lv_example_label_1(); */
  /* - etc. */
  //lv_demo_widgets();

  lv_obj_t * obj_tela = screen_animations_create();
  lv_screen_load(obj_tela);

/*

    /* 2. Teste Simples (Sem seu código)
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "TESTE: O Simulador Funciona!");
    lv_obj_center(label);

*/

/*
// --- TESTE CRÍTICO ---

    // 1. Tente criar uma NOVA tela (igual o arquivo gerado faz)
    // Se travar aqui, o problema é o NULL.
    lv_obj_t * nova_tela = lv_obj_create(NULL);

    if (nova_tela == NULL) {
        printf("ERRO: Falha ao criar tela!\n");
    } else {
        // 2. Se criou, vamos tentar carregar
        lv_screen_load(nova_tela);

        // 3. Criar um label nela
        lv_obj_t * label = lv_label_create(nova_tela);
        lv_label_set_text(label, "Nova Tela Funcionou!");
        lv_obj_center(label);
    }

  */

    // ---------------------

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if(sleep_time_ms == LV_NO_TIMER_READY){
	sleep_time_ms =  LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}


#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
