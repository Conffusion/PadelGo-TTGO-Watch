#include "config.h"
#include <TTGO.h>
#include "hardware/motor.h"

#include "runcoach_app.h"
#include "runcoach_app_main.h"
#include "runcoach_app_model.h"
#include "runcoach_app_gui.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include <hardware/ble/gadgetbridge.h>
static lv_task_t *runcoach_runtime_task = NULL;

static short TIME_INCREASE=30;

// Forward declaration
void runcoach_runtime_update_task(lv_task_t *task);

static short remaining_time() {
    short remaining_seconds = 0;
    if(runTimeSchema.currSectionIdx>=0) {
        time_t now = time(0);
        double dif_seconds = difftime(now,runTimeSchema.currSectionStart);
        remaining_seconds = runTimeSchema.sections[runTimeSchema.currSectionIdx].duration - static_cast<short>(dif_seconds);
    } else {
        if(runTimeSchema.nrOfSections>0) {
            remaining_seconds = runTimeSchema.sections[0].duration;
        }
    }
    return remaining_seconds;
}

/**
 * Is called every second when the run schema is running to update the remaining time label.
 */
void runcoach_runtime_update_task(lv_task_t *task) {
    runTimeSchema.remainingTime = remaining_time();
    if(runTimeSchema.status == RUNNING && runTimeSchema.remainingTime<=0) {
        // move to next section
        runTimeSchema.currSectionIdx++;
        if(runTimeSchema.currSectionIdx>=runTimeSchema.nrOfSections) {
            // schema finished
            runTimeSchema.currSectionIdx=-1;
            runTimeSchema.status=STOPPED;
            runcoach_update_labels();
            motor_vibe(300);
            if(task) {
                lv_task_del(task);
            }
            runcoach_runtime_task = NULL;
            return;
        } else {
            // start next section
            runTimeSchema.currSectionStart=time(0);
            runcoach_update_labels();
            motor_vibe(100);
        }
    } else {
        runcoach_update_labels();
    }
}

static void runcoach_stop_runtime_task() {
    if(runcoach_runtime_task) {
        lv_task_del(runcoach_runtime_task);
        runcoach_runtime_task = NULL;
        Serial.println("Runtime task stopped");
    }
}
static void runcoach_start_runtime_task() {
    if(!runcoach_runtime_task) {
        Serial.println("Creating runtime task");
        runcoach_runtime_task = lv_task_create(runcoach_runtime_update_task, 1000, LV_TASK_PRIO_MID, NULL);
    }
}

void enter_run_time_screen_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if(runTimeSchema.nrOfSections <= 0) {
                return;
            }
            runcoach_show_runtime_screen();
            break;
    }
}

void exit_run_time_screen_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            runcoach_show_main_screen();
            break;
    }
}

void enter_schema_add_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            // Append runtime sections from current run/walk/repeat values
            runcoach_stop_runtime_task();
            runcoach_appendRunSchema();
            runTimeSchema.status = STOPPED;
            runTimeSchema.currSectionIdx = -1;
            runTimeSchema.remainingTime = (runTimeSchema.nrOfSections > 0 && runTimeSchema.sections)
                ? runTimeSchema.sections[0].duration
                : 0;
            runcoach_update_labels();
            runcoach_update_sections_label();
            runcoach_set_run_time_open_btn_status(true);
            Serial.println("RunCoach sections added");
            break;
    }
}

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
                    runcoach_start_runtime_task();
                    break;
                case (RUNNING):
                    runcoach_pauze_schema();
                    runcoach_stop_runtime_task();
                    break;
                case (PAUZING):
                    runcoach_continue_schema();
                    runcoach_start_runtime_task();
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

void exit_runcoach_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            runcoach_stop_runtime_task();
            // Reset model to default values before exiting
            runcoach_model_init();
            runcoach_update_labels();
            runcoach_update_sections_label();
            mainbar_jump_back();
            break;
    }
}
