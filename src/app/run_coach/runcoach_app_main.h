#ifndef _RUNCOACH_APP_MAIN_H
    #define _RUNCOACH_APP_MAIN_H
    #include "lvgl.h"

    void enter_schema_run_min_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_run_plus_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_walk_min_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_walk_plus_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_repeat_min_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_repeat_plus_event_cb( lv_obj_t * obj, lv_event_t event );

    void enter_run_time_screen_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_schema_add_event_cb( lv_obj_t * obj, lv_event_t event );
    void exit_run_time_screen_event_cb( lv_obj_t * obj, lv_event_t event );
    void enter_run_time_action_event_cb( lv_obj_t * obj, lv_event_t event );

    void enter_runcoach_app_reset_event_cb( lv_obj_t * obj, lv_event_t event );
    void exit_runcoach_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

    extern lv_obj_t *schema_run_time_label;
    extern lv_obj_t *schema_walk_time_label;
    extern lv_obj_t *schema_repeat_value_label;
    extern lv_obj_t *run_time_action_label;
    extern lv_obj_t *run_time_clock_label;
    extern lv_obj_t *run_time_action_btn;

    /**
     * @brief setup main runcoach main tile
     * 
     * @param tile_num return tile for runcoach main tile
     */
    void runcoach_app_main_setup( uint32_t tile_num );

#endif // _RUNCOACH_APP_MAIN_H