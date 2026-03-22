#include "runcoach_app_gui.h"
#include "runcoach_app_main.h"
#include "runcoach_app_model.h"
#include "hardware/motor.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "lvgl.h"

lv_obj_t *schema_run_min_btn=NULL;
lv_obj_t *schema_run_plus_btn=NULL;
lv_obj_t *schema_run_time_label=NULL;
lv_obj_t *schema_walk_min_btn=NULL;
lv_obj_t *schema_walk_plus_btn=NULL;
lv_obj_t *schema_walk_time_label=NULL;
lv_obj_t *schema_repeat_min_btn=NULL;
lv_obj_t *schema_repeat_plus_btn=NULL;
lv_obj_t *schema_repeat_value_label=NULL;

lv_obj_t *schema_sections_value_label=NULL;
lv_obj_t *run_time_action_label=NULL;
lv_obj_t *run_time_clock_label=NULL;
lv_obj_t *run_time_steps_label=NULL;
lv_obj_t *run_time_action_btn=NULL;
static lv_obj_t *run_time_open_btn=NULL;
static lv_obj_t *runcoach_main_screen_container = NULL;
static lv_obj_t *runcoach_runtime_screen_container = NULL;

LV_IMG_DECLARE(up_16px);

lv_obj_t *runcoach_app_main_tile = NULL;

// Styles
static lv_style_t runcoach_app_main_style;
static lv_style_t large_label_style;
static lv_style_t medium_label_style;
static lv_style_t transparent_style;

char runDurationLabel[5];
char walkDurationLabel[5];
char remainingTimeLabel[5];

LV_FONT_DECLARE(Ubuntu_48px);
LV_FONT_DECLARE(Ubuntu_32px);


static void runcoach_initialize_styles(uint32_t tile_num) {
    runcoach_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &runcoach_app_main_style, APP_STYLE );

    lv_style_init(&large_label_style);
    lv_style_copy(&large_label_style, APP_STYLE);
    lv_style_set_text_font(&large_label_style, LV_STATE_DEFAULT, &Ubuntu_48px);
    lv_style_set_text_color(&large_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
 
    lv_style_init(&medium_label_style);
    lv_style_copy(&medium_label_style, APP_STYLE);
    lv_style_set_text_font(&medium_label_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&medium_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
 
    lv_style_init(&transparent_style);
    lv_style_copy(&transparent_style, APP_STYLE);
    lv_style_set_bg_opa(&transparent_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&transparent_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
}

// Loop   <-> [00:00] <+>
static void schema_run_bar_setup(lv_obj_t * parent, int height) {
    lv_obj_t * schema_run_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_run_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * schema_run_label = wf_add_label(schema_run_container, "R");
    lv_obj_add_style(schema_run_label, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_run_min_btn = wf_add_button_c(schema_run_container,"-",LV_HOR_RES/7, height, enter_schema_run_min_event_cb);
    lv_obj_add_style(schema_run_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_run_time_label = wf_add_label(schema_run_container, "00:00" );
    lv_obj_add_style(schema_run_time_label, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_run_plus_btn = wf_add_button_c(schema_run_container,"+",LV_HOR_RES/7, height, enter_schema_run_plus_event_cb);
    lv_obj_add_style(schema_run_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
}

// Stap   <-> [00:00] <+>
static void schema_walk_bar_setup(lv_obj_t * parent, int height) {
    lv_obj_t * schema_walk_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_walk_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * schema_walk_label = wf_add_label(schema_walk_container, "W");
    lv_obj_add_style(schema_walk_label, LV_LABEL_PART_MAIN, &medium_label_style);
    
    schema_walk_min_btn = wf_add_button_c(schema_walk_container,"-",LV_HOR_RES/8, height, enter_schema_walk_min_event_cb);
    lv_obj_add_style(schema_walk_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_walk_time_label = wf_add_label(schema_walk_container, "00:00" );
    lv_obj_set_width(schema_walk_time_label, LV_HOR_RES/5);
    lv_obj_add_style(schema_walk_time_label, LV_LABEL_PART_MAIN, &medium_label_style);
    
    schema_walk_plus_btn = wf_add_button_c(schema_walk_container,"+",LV_HOR_RES/7, height, enter_schema_walk_plus_event_cb);
    lv_obj_add_style(schema_walk_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
}

// Herhaal   <-> [  0] <+>
static void schema_repeat_bar_setup(lv_obj_t * parent, int height) {
    lv_obj_t * schema_repeat_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_repeat_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * schema_repeat_label = wf_add_label(schema_repeat_container, "X");
    lv_obj_add_style(schema_repeat_label, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_repeat_min_btn = wf_add_button_c(schema_repeat_container,"-",LV_HOR_RES/7, height, enter_schema_repeat_min_event_cb);
    lv_obj_add_style(schema_repeat_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_repeat_value_label = wf_add_label(schema_repeat_container, "0" );
    lv_obj_add_style(schema_repeat_value_label, LV_LABEL_PART_MAIN, &medium_label_style);

    schema_repeat_plus_btn = wf_add_button_c(schema_repeat_container,"+",LV_HOR_RES/7, height, enter_schema_repeat_plus_event_cb);
    lv_obj_add_style(schema_repeat_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
}

// <Exit> <+> <Go (0)>
static void schema_button_bar_setup(lv_obj_t * parent) {
    lv_obj_t * schema_buttonbar_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_buttonbar_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    wf_add_exit_button(schema_buttonbar_container, exit_runcoach_app_main_event_cb);
    wf_add_add_button(schema_buttonbar_container, enter_schema_add_event_cb);
    run_time_open_btn = wf_add_button_c(schema_buttonbar_container,"Go (0)",LV_HOR_RES/4,LV_VER_RES/6,enter_run_time_screen_event_cb);
    runcoach_set_run_time_open_btn_status(DISABLED);
}

/*
    _______________________________
    |         |     |       |     |
    | Loop    |  -  | 00:00 |  +  | // run bar     \
    |_________|_____|_______|_____|                 \
    |         |     |       |     |                  \
    | Stap    |  -  | 00:00 |  +  | // Walk bar       | Schema 
    |_________|_____|_______|_____|                  /
    |         |     |       |     |                 /
    | Herhaal |  -  |     0 |  +  | // Repeat bar  /
    |_________|_____|_______|_____|
    |  <Exit>   <+>      <Go (0)> | // schema buttonbar
    |_____________________________|

*/
static void schema_setup(lv_obj_t * parent) {
    int height = LV_VER_RES/8;
    lv_obj_t * schema_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_container, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    schema_run_bar_setup(schema_container, height);
    schema_walk_bar_setup(schema_container, height);
    schema_repeat_bar_setup(schema_container, height);
    schema_button_bar_setup(schema_container);
}


/*
     ___________________
    |      <action>    |
    |                  |
    |      00:00       |
    |  <steps>         |
    |  <btn>  <Back>   | // run_time bar ; btn=Setup|Start|Pauze
    |__________________|
 */
static void run_time_screen_setup(lv_obj_t * parent) {
    runcoach_runtime_screen_container = wf_add_container(parent, LV_LAYOUT_COLUMN_MID, LV_FIT_PARENT, LV_FIT_PARENT, false);
    lv_obj_align(runcoach_runtime_screen_container, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    run_time_action_label = wf_add_label(runcoach_runtime_screen_container, "---",&medium_label_style);
    lv_obj_set_width(run_time_action_label, LV_HOR_RES);
    lv_label_set_align(run_time_action_label, LV_LABEL_ALIGN_CENTER);

    run_time_clock_label = wf_add_label(runcoach_runtime_screen_container, "00:00", &large_label_style);
    lv_obj_set_width(run_time_clock_label, LV_HOR_RES);
    lv_label_set_align(run_time_clock_label, LV_LABEL_ALIGN_CENTER);

    run_time_steps_label = wf_add_label(runcoach_runtime_screen_container, "Steps: 0", &medium_label_style);
    lv_obj_set_width(run_time_steps_label, LV_HOR_RES);
    lv_label_set_align(run_time_steps_label, LV_LABEL_ALIGN_CENTER);

    lv_obj_t *run_time_button_container = wf_add_container(runcoach_runtime_screen_container, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    run_time_action_btn = wf_add_button_c(run_time_button_container, "Start", LV_HOR_RES/3 - 8, LV_VER_RES/6, enter_run_time_action_event_cb);
    wf_add_button_c(run_time_button_container, "Return", LV_HOR_RES/3 - 8, LV_VER_RES/6, exit_run_time_screen_event_cb);

    lv_obj_set_hidden(runcoach_runtime_screen_container, true);
}

void to_time_label(short seconds, char* dest) {
    // minutes
    int min = (seconds / 60) % 60;
    // seconds
    int sec = seconds - min*60;
    sprintf(dest,"%02d:%02d", min, sec);
}

void runcoach_update_labels() {
    to_time_label(schemaDef.runTime,runDurationLabel);
    lv_label_set_text(schema_run_time_label,  runDurationLabel);
    to_time_label(schemaDef.walkTime,walkDurationLabel);
    lv_label_set_text(schema_walk_time_label, walkDurationLabel);
    lv_label_set_text_fmt(schema_repeat_value_label,"%d",schemaDef.repeat);
    if(runTimeSchema.nrOfSections > 0) {
        runcoach_set_run_time_open_btn_status(true);
    } else {
        runcoach_set_run_time_open_btn_status(false);
    }
    if(run_time_action_label && run_time_clock_label && run_time_action_btn) {
        if(runTimeSchema.currSectionIdx>=0 && runTimeSchema.sections != nullptr) {
            lv_label_set_text(run_time_action_label, runTimeSchema.sections[runTimeSchema.currSectionIdx].actionLabel);
            lv_label_set_text_fmt(run_time_steps_label, "Steps: %d", runTimeSchema.sections[runTimeSchema.lastRunSectionIdx].steps);
        } else {
            lv_label_set_text(run_time_action_label,"---");
            lv_label_set_text(run_time_steps_label,"Steps: 0");
        }
        to_time_label(runTimeSchema.remainingTime,remainingTimeLabel);
        lv_label_set_text(run_time_clock_label,remainingTimeLabel);

        lv_obj_t *action_btn_label = lv_obj_get_child(run_time_action_btn, NULL);
        if(action_btn_label) {
            lv_label_set_text(action_btn_label, to_status_label(runTimeSchema.status));
        }
    }
}

void runcoach_show_runtime_screen() {
    if(runcoach_main_screen_container && runcoach_runtime_screen_container) {
        lv_obj_set_hidden(runcoach_main_screen_container, true);
        lv_obj_set_hidden(runcoach_runtime_screen_container, false);
        runcoach_update_labels();
    }
}

void runcoach_show_main_screen() {
    if(runcoach_main_screen_container && runcoach_runtime_screen_container) {
        lv_obj_set_hidden(runcoach_runtime_screen_container, true);
        lv_obj_set_hidden(runcoach_main_screen_container, false);
    }
}

void runcoach_update_sections_label() {
    lv_obj_t *action_btn_label = lv_obj_get_child(run_time_open_btn, NULL);
    if(action_btn_label) {
        Serial.println("Updating run time open button label with sections count: " + String(runTimeSchema.nrOfSections));
        lv_label_set_text_fmt(action_btn_label, "Go (%d) >", runTimeSchema.nrOfSections/2);
    }
}

void runcoach_set_run_time_open_btn_status(bool enabled) {
    if(!run_time_open_btn) {
        return;
    }
    if(enabled) {
        lv_obj_set_style_local_bg_opa(run_time_open_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_obj_set_style_local_bg_color(run_time_open_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    } else {
        lv_obj_set_style_local_bg_opa(run_time_open_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_obj_set_style_local_bg_color(run_time_open_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    }
}

void runcoach_app_gui_setup( uint32_t tile_num ) {
    runcoach_initialize_styles(tile_num);
    runcoach_main_screen_container = wf_add_container( runcoach_app_main_tile, LV_LAYOUT_COLUMN_MID, LV_FIT_PARENT, LV_FIT_PARENT, false );
    schema_setup(runcoach_main_screen_container);
    run_time_screen_setup(runcoach_app_main_tile);
    runcoach_update_labels();
}