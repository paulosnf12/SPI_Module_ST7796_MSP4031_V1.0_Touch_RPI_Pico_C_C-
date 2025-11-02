/**
 * @file button_show_numpad_gen.h
 */

#ifndef BUTTON_SHOW_NUMPAD_H
#define BUTTON_SHOW_NUMPAD_H

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
    BUTTON_SHOW_NUMPAD_TIMELINE_SHOW_UP = 0,
    _BUTTON_SHOW_NUMPAD_TIMELINE_CNT = 1
}button_show_numpad_timeline_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * button_show_numpad_create(lv_obj_t * parent, const void * button_image, bool toggle, bool checked);

/**
 * Get a timeline of a button_show_numpad
 * @param obj          pointer to a button_show_numpad component
 * @param timeline_id  ID of the the timeline
 * @return             pointer to the timeline or NULL if not found
 */
lv_anim_timeline_t * button_show_numpad_get_timeline(lv_obj_t * obj, button_show_numpad_timeline_t timeline_id);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*BUTTON_SHOW_NUMPAD_H*/