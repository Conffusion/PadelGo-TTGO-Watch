#include "config.h"

#include "runcoach_app.h"
#include "runcoach_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"
/*
 * app tiles
 */
uint32_t runcoach_app_main_tile_num;
/*
 * app icon
 */
icon_t *runcoach_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(runcoach_app_64px);
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &runcoach_app_setup, 1 );           /** @brief app autocall function */
/*
 * setup routine for example app
 */
void runcoach_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    /*
     * register app
     */
    runcoach_app_main_tile_num = mainbar_add_app_tile( 1, 1, "run coach app" );
    runcoach_app = app_register( "Run Coach", &runcoach_app_64px, enter_runcoach_app_event_cb );
    runcoach_app_main_setup( runcoach_app_main_tile_num );
    Serial.begin(115200); // Standard baud rate for ESP32/T-Watch
    Serial.println("RunCoach App Started");
}
/**
 * @brief Get the app main tile num object
 * 
 * @return uint32_t 
 */
uint32_t runcoach_app_get_app_main_tile_num( void ) {
    return( runcoach_app_main_tile_num );
}
/**
 * @brief call back function for enter app
 * 
 * @param obj           object
 * @param event         event
 */
void enter_runcoach_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( runcoach_app_main_tile_num, LV_ANIM_OFF, true );
                                        app_hide_indicator( runcoach_app );
                                        break;
    }
}
/**
 * @brief call back function for exit app
 * 
 * @param obj           object
 * @param event         event
 */
void exit_runcoach_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}