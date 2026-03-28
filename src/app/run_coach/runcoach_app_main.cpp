#include "config.h"
#include <TTGO.h>
#include "hardware/motor.h"
#include "hardware/sound.h"
#include "hardware/display.h"

#include "runcoach_app.h"
#include "runcoach_app_main.h"
#include "runcoach_app_model.h"
#include "runcoach_app_gui.h"

#include "gui/sound/piep.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "hardware/motion.h"
#include <hardware/ble/gadgetbridge.h>
static lv_task_t *runcoach_runtime_task = NULL;

static short TIME_INCREASE=30;
static const uint32_t RUNCOACH_DIM_DELAY_SECONDS = 10;
static const uint32_t RUNCOACH_DIM_PERCENT = 2;
static const uint32_t RUNCOACH_LAST_SECONDS = 10;
static const uint32_t RUNCOACH_END_SECTION_PERCENT = 50;
static bool runcoach_brightness_saved = false;
static uint32_t runcoach_saved_brightness = DISPLAY_MAX_BRIGHTNESS;

enum RuncoachBrightnessState {
    RUNCOACH_BRIGHTNESS_NORMAL = 0,
    RUNCOACH_BRIGHTNESS_DIM,
    RUNCOACH_BRIGHTNESS_BOOST
};

static RuncoachBrightnessState runcoach_section_brightness_state = RUNCOACH_BRIGHTNESS_NORMAL;

static uint32_t runcoach_dim_brightness_value() {
    uint32_t value = ( DISPLAY_MAX_BRIGHTNESS * RUNCOACH_DIM_PERCENT ) / 100;
    if ( value < DISPLAY_MIN_BRIGHTNESS ) {
        value = DISPLAY_MIN_BRIGHTNESS;
    }
    return value;
}

static uint32_t runcoach_boost_brightness_value() {
    uint32_t value = ( DISPLAY_MAX_BRIGHTNESS * RUNCOACH_END_SECTION_PERCENT ) / 100;
    if ( value < DISPLAY_MIN_BRIGHTNESS ) {
        value = DISPLAY_MIN_BRIGHTNESS;
    }
    return value;
}

static void runcoach_save_brightness_if_needed() {
    if ( !runcoach_brightness_saved ) {
        runcoach_saved_brightness = display_get_brightness();
        runcoach_brightness_saved = true;
    }
}

static void runcoach_restore_brightness() {
    if ( runcoach_brightness_saved ) {
        display_set_brightness( runcoach_saved_brightness );
    }
    runcoach_section_brightness_state = RUNCOACH_BRIGHTNESS_NORMAL;
}

static void runcoach_on_section_start() {
    runcoach_save_brightness_if_needed();
    runcoach_restore_brightness();
}

static void runcoach_handle_section_dimming() {
    if ( runTimeSchema.status != RUNNING || runTimeSchema.currSectionIdx < 0 || runTimeSchema.sections == nullptr ) {
        return;
    }

    Section &currentSection = runTimeSchema.sections[ runTimeSchema.currSectionIdx ];
    RuncoachBrightnessState wanted_state = RUNCOACH_BRIGHTNESS_NORMAL;

    if ( currentSection.action == RUN && runTimeSchema.remainingTime > 0 && runTimeSchema.remainingTime <= RUNCOACH_LAST_SECONDS ) {
        wanted_state = RUNCOACH_BRIGHTNESS_BOOST;
    }

    time_t now = time( 0 );
    if ( wanted_state == RUNCOACH_BRIGHTNESS_NORMAL && difftime( now, runTimeSchema.currSectionStart ) >= RUNCOACH_DIM_DELAY_SECONDS ) {
        wanted_state = RUNCOACH_BRIGHTNESS_DIM;
    }

    if ( runcoach_section_brightness_state == wanted_state ) {
        return;
    }

    switch ( wanted_state ) {
        case RUNCOACH_BRIGHTNESS_DIM:
            display_set_brightness( runcoach_dim_brightness_value() );
            break;
        case RUNCOACH_BRIGHTNESS_BOOST:
            display_set_brightness( runcoach_boost_brightness_value() );
            break;
        case RUNCOACH_BRIGHTNESS_NORMAL:
        default:
            runcoach_restore_brightness();
            break;
    }

    runcoach_section_brightness_state = wanted_state;
}

// Forward declaration
void runcoach_runtime_update_task(lv_task_t *task);

static short runcoach_remaining_time() {
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
static void runcoach_update_current_section_steps() {
    if(runTimeSchema.currSectionIdx>=0 && runTimeSchema.sections) {
        Section& currentSection = runTimeSchema.sections[runTimeSchema.currSectionIdx];
        if(currentSection.action == RUN) {
            currentSection.steps = bma_get_stepcounter() - runTimeSchema.currSectionStepsStart;
        }
    }    
}

/**
 * Is called every second when the run schema is running to update the remaining time label.
 */
void runcoach_runtime_update_task(lv_task_t *task) {
    runTimeSchema.remainingTime = runcoach_remaining_time();
    runcoach_update_current_section_steps();
    runcoach_handle_section_dimming();
    if(runTimeSchema.status == RUNNING) { 
        if(runTimeSchema.remainingTime<=0) {
            if ( sound_get_available() && sound_get_enabled_config() ) {
                sound_play_progmem_wav( piep_wav, piep_wav_len );
            }
            // move to next section
            runTimeSchema.currSectionIdx++;
            if(runTimeSchema.currSectionIdx>=runTimeSchema.nrOfSections) {
                // schema finished
                runTimeSchema.currSectionIdx=-1;
                runTimeSchema.status=STOPPED;
                runcoach_restore_brightness();
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
                runTimeSchema.currSectionStepsStart=bma_get_stepcounter();
                if(runTimeSchema.sections[runTimeSchema.currSectionIdx].action == RUN) {
                    runTimeSchema.lastRunSectionIdx=runTimeSchema.currSectionIdx;
                }
                runcoach_on_section_start();
                runcoach_update_labels();
                motor_vibe(200);
            }
        } else if(runTimeSchema.remainingTime<=10) {
            // vibrate when 10 seconds or less remaining in current section
            motor_vibe(30);
        }
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
            runcoach_restore_brightness();
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
            if(schemaDef.runTime>=TIME_INCREASE) {
                schemaDef.runTime-=TIME_INCREASE;
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
            if(schemaDef.walkTime>=TIME_INCREASE) {
                schemaDef.walkTime-=TIME_INCREASE;
                runcoach_update_labels();
            }
    }
}
void enter_schema_walk_plus_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("walk increase pressed");
            schemaDef.walkTime+=TIME_INCREASE;
            runcoach_update_labels();
    }
}
void enter_schema_repeat_min_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("repeat decrease pressed");
            if(schemaDef.repeat>=1) {
                schemaDef.repeat-=1;
                runcoach_update_labels();
            }
    }
}
void enter_schema_repeat_plus_event_cb( lv_obj_t * obj, lv_event_t event ){
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            Serial.println("repeat increase pressed");
            schemaDef.repeat+=1;
            runcoach_update_labels();
    }
}
void enter_run_time_action_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            switch(runTimeSchema.status) {
                case(STOPPED):
                    runcoach_launch_schema();
                    runcoach_on_section_start();
                    runcoach_start_runtime_task();
                    break;
                case (RUNNING):
                    runcoach_pauze_schema();
                    runcoach_restore_brightness();
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
            runcoach_restore_brightness();
            // Reset model to default values before exiting
            runcoach_model_init();
            runcoach_update_labels();
            runcoach_update_sections_label();
            mainbar_jump_back();
            break;
    }
}
