#include "config.h"

#include "scoreboard_app.h"
#include "scoreboard_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"
/*
 * app tiles
 */
uint32_t scoreboard_app_main_tile_num;
/*
 * app icon
 */
icon_t *scoreboard_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(scoreboard_app_64px);
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &scoreboard_app_setup, 4 );           /** @brief app autocall function */
/*
 * setup routine for example app
 */
void scoreboard_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    /*
     * register app
     */
    scoreboard_app_main_tile_num = mainbar_add_app_tile( 1, 1, "scoreboard app" );
    scoreboard_app = app_register( "Scoreboard", &scoreboard_app_64px, enter_scoreboard_app_event_cb );
    scoreboard_app_main_setup( scoreboard_app_main_tile_num );

}
/**
 * @brief Get the app main tile num object
 * 
 * @return uint32_t 
 */
uint32_t scoreboard_app_get_app_main_tile_num( void ) {
    return( scoreboard_app_main_tile_num );
}
/**
 * @brief call back function for enter app
 * 
 * @param obj           object
 * @param event         event
 */
void enter_scoreboard_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( scoreboard_app_main_tile_num, LV_ANIM_OFF, true );
                                        app_hide_indicator( scoreboard_app );
                                        break;
    }
}
/**
 * @brief call back function for exit app
 * 
 * @param obj           object
 * @param event         event
 */
void exit_scoreboard_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}