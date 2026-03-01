#ifndef _RUNCOACH_APP_H
    #define _RUNCOACH_APP_H

    /**
     * @brief setup runcoach app
     * 
     */
    void runcoach_app_setup( void );
    /**
     * @brief get to runcoach app tile number
     * 
     * @return uint32_t tilenumber
     */
    uint32_t runcoach_app_get_app_main_tile_num( void );
    /**
     * @brief call back function when enter the app
     * 
     * @param obj           object
     * @param event         event
     */
    void enter_runcoach_app_event_cb( lv_obj_t * obj, lv_event_t event );
    /**
     * @brief call back function when exit the app
     * 
     * @param obj           object
     * @param event         event
     */
    void exit_runcoach_app_event_cb( lv_obj_t * obj, lv_event_t event );

#endif // _RUNCOACH_APP_H