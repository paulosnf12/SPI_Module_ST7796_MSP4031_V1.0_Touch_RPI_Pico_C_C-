/**
 * @file ui_animations_gen.h
 */

#ifndef UI_ANIMATIONS_GEN_H
#define UI_ANIMATIONS_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define DARK_BLUE lv_color_hex(0x003762)

#define YELLOW lv_color_hex(0xda9d19)

#define UNIT_SMALL 6

#define UNIT_MEDIUM 12

#define UNIT_LARGE 24

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

extern const void * seta_esquerda;

/*----------------
 * Subjects
 *----------------*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void button_click_event(lv_event_t * e);
void enter_button_event(lv_event_t * e);

/**
 * Initialize the component library
 */

void ui_animations_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widget and components of this library*/
#include "components/buttons/button_normal_gen.h"
#include "components/buttons/button_show_numpad_gen.h"
#include "components/list/list_gen.h"
#include "screens/screen_animations_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ANIMATIONS_GEN_H*/