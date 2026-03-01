/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include <TTGO.h>

#include "runcoach_app.h"
#include "runcoach_app_main.h"
#include "runcoach_app_model.h"
#include "runcoach_app_gui.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include <hardware/ble/gadgetbridge.h>

static short TIME_INCREASE=30;

void enter_schema_run_min_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("run decrease pressed");
            if(schemaDef.runTime>30) {
                schemaDef.runTime=schemaDef.runTime-TIME_INCREASE;
                runcoach_update_labels();
            }
    }
}
void enter_schema_run_plus_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("run increase pressed");
            schemaDef.runTime+=TIME_INCREASE;
            runcoach_update_labels();
    }
}
void enter_schema_walk_min_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("walk decrease pressed");
            if(schemaDef.walkTime>30) {
                schemaDef.walkTime=schemaDef.walkTime-TIME_INCREASE;
                runcoach_update_labels();
            }
    }
}
void enter_schema_walk_plus_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("walk increase pressed");
            schemaDef.walkTime=schemaDef.walkTime+TIME_INCREASE;
            runcoach_update_labels();
    }
}
void enter_schema_repeat_min_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("repeat decrease pressed");
            if(schemaDef.repeat>0) {
                schemaDef.repeat=schemaDef.repeat-1;
                runcoach_update_labels();
            }
    }
}
void enter_schema_repeat_plus_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("repeat increase pressed");
            schemaDef.repeat=schemaDef.repeat+1;
            runcoach_update_labels();
    }
}
void enter_run_time_action_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            switch(runTimeSchema.status) {
                case(STOPPED):
                    runcoach_launch_schema();
                    break;
                case (RUNNING):
                    runcoach_pauze_schema();
                    break;
                case (PAUZING):
                    runcoach_continue_schema();
                    break;
            };
            runcoach_update_labels();
            break;
    }
}
// GUI setup
void runcoach_app_main_setup( uint32_t tile_num ) {
    runcoach_app_gui_setup(tile_num); // Ensure setup is called to initialize styles
}

void enter_runcoach_app_reset_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            runcoach_model_init();
            runcoach_update_labels();
    }
}

void exit_runcoach_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_back();
            break;
    }
}