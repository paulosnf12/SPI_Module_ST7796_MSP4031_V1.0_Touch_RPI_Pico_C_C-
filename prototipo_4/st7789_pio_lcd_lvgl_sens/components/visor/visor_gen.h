/**
 * @file visor_gen.h
 */

#ifndef VISOR_H
#define VISOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "../../lvgl/lvgl.h"  
#endif

/*********************
 *      DEFINES
 *********************/



/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    VISOR_TIMELINE_DISPLAY_FADE_IN = 0,
    _VISOR_TIMELINE_CNT = 1
}visor_timeline_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * visor_create(lv_obj_t * parent);

void visor_set_value(const char * text);

void visor_set_enter_callback(void (*cb)(float));

void visor_manual_enter(float v);



/**
 * Get a timeline of a visor
 * @param obj          pointer to a visor component
 * @param timeline_id  ID of the the timeline
 * @return             pointer to the timeline or NULL if not found
 */
lv_anim_timeline_t * visor_get_timeline(lv_obj_t * obj, visor_timeline_t timeline_id);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VISOR_H*/