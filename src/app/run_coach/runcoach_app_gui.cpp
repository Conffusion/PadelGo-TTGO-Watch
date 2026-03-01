#include "runcoach_app_gui.h"
#include "runcoach_app_main.h"
#include "runcoach_app_model.h"

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
lv_obj_t *run_time_action_label=NULL;
lv_obj_t *run_time_clock_label=NULL;
lv_obj_t *run_time_action_btn=NULL;
LV_IMG_DECLARE(up_16px);

lv_obj_t *runcoach_app_main_tile = NULL;

// Styles
static lv_style_t runcoach_app_main_style;
//static lv_style_t large_label_style;
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

//    lv_style_init(&large_label_style);
//    lv_style_copy(&large_label_style, APP_STYLE);
//    lv_style_set_text_font(&large_label_style, LV_STATE_DEFAULT, &Ubuntu_48px);
//    lv_style_set_text_color(&large_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
 
    lv_style_init(&medium_label_style);
    lv_style_copy(&medium_label_style, APP_STYLE);
    lv_style_set_text_font(&medium_label_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&medium_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
 
    lv_style_init(&transparent_style);
    lv_style_copy(&transparent_style, APP_STYLE);
    lv_style_set_bg_opa(&transparent_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&transparent_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
}

static void schema_run_bar_setup(lv_obj_t * parent) {
    lv_obj_t * schema_run_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_run_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * schema_run_label = wf_add_label(schema_run_container, "Run");
    lv_obj_set_width(schema_run_label, LV_HOR_RES/4);

    schema_run_min_btn = wf_add_button_c(schema_run_container,"-",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_run_min_event_cb);
    lv_obj_add_style(schema_run_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_run_min_btn, LV_BTN_PART_MAIN, &transparent_style);

    schema_run_time_label = wf_add_label(schema_run_container, "00:00" );
    lv_obj_set_width(schema_run_time_label, LV_HOR_RES/4);

    schema_run_plus_btn = wf_add_button_c(schema_run_container,"+",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_run_plus_event_cb);
    lv_obj_add_style(schema_run_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_run_plus_btn, LV_BTN_PART_MAIN, &transparent_style);
}

static void schema_walk_bar_setup(lv_obj_t * parent) {
    lv_obj_t * schema_walk_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_walk_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * walk_label = wf_add_label(schema_walk_container, "Walk");
    lv_obj_set_width(walk_label, LV_HOR_RES/4);

    schema_walk_min_btn = wf_add_button_c(schema_walk_container,"-",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_walk_min_event_cb);
    lv_obj_add_style(schema_walk_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_walk_min_btn, LV_BTN_PART_MAIN, &transparent_style);

    schema_walk_time_label = wf_add_label(schema_walk_container, "00:00" );
    lv_obj_set_width(schema_walk_time_label, LV_HOR_RES/4);

    schema_walk_plus_btn = wf_add_button_c(schema_walk_container,"+",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_walk_plus_event_cb);
    lv_obj_add_style(schema_walk_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_walk_plus_btn, LV_BTN_PART_MAIN, &transparent_style);
}

static void schema_repeat_bar_setup(lv_obj_t * parent) {
    lv_obj_t * schema_repeat_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_repeat_container, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_t * schema_repeat_label = wf_add_label(schema_repeat_container, "Repeat");
    lv_obj_set_width(schema_repeat_label, LV_HOR_RES/4);

    schema_repeat_min_btn = wf_add_button_c(schema_repeat_container,"-",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_repeat_min_event_cb);
    lv_obj_add_style(schema_repeat_min_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_repeat_min_btn, LV_BTN_PART_MAIN, &transparent_style);

    schema_repeat_value_label = wf_add_label(schema_repeat_container, "0" );
    lv_obj_set_width(schema_repeat_label, LV_HOR_RES/4);

    schema_repeat_plus_btn = wf_add_button_c(schema_repeat_container,"+",LV_HOR_RES/5, LV_VER_RES/5, enter_schema_repeat_plus_event_cb);
    lv_obj_add_style(schema_repeat_plus_btn, LV_LABEL_PART_MAIN, &medium_label_style);
    lv_obj_add_style(schema_repeat_plus_btn, LV_BTN_PART_MAIN, &transparent_style);
}

static void schema_setup(lv_obj_t * parent) {
    lv_obj_t * schema_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(schema_container, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    schema_run_bar_setup(schema_container);
    schema_walk_bar_setup(schema_container);
    schema_repeat_bar_setup(schema_container);
}

static void run_time_bar_setup(lv_obj_t * parent) {
    lv_obj_t * run_time_container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false);
    lv_obj_align(run_time_container, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    run_time_action_label = wf_add_label(run_time_container,"---");
    lv_obj_set_width(run_time_action_label, LV_HOR_RES/4);
    run_time_clock_label = wf_add_label(run_time_container,"00:00");
    lv_obj_set_width(run_time_clock_label,LV_HOR_RES/4);
    run_time_action_btn = wf_add_button_c(run_time_container,"Setup",LV_HOR_RES/4,LV_VER_RES/5,enter_run_time_action_event_cb);
}

static void actionbar_setup(lv_obj_t * parent) {
    lv_obj_t * actionbar_container = wf_add_container( parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false );
    lv_obj_align(actionbar_container, parent, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0 );

    // Reset run time status
    wf_add_refresh_button(actionbar_container, enter_runcoach_app_reset_event_cb);
    // Exit button
    wf_add_exit_button(actionbar_container, exit_runcoach_app_main_event_cb);
}

void to_time_label(short seconds, char* dest) {
    // minutes
    int min = (seconds / 60) % 60;

    // seconds
    int sec = seconds - min*60;
   
    sprintf(dest,"%02d:%02d", min, sec);
    Serial.print(dest);
}

static void remaining_time(RunTimeSchema runtime) {
    if(runtime.currSectionIdx>=0) {
        time_t now = time(0);
        double dif_seconds = difftime(now,runtime.currSectionStart);
        to_time_label(static_cast<short>(dif_seconds),remainingTimeLabel);
    }
}
void runcoach_update_labels() {
    to_time_label(schemaDef.runTime,runDurationLabel);
    lv_label_set_text(schema_run_time_label,  runDurationLabel);
    to_time_label(schemaDef.walkTime,walkDurationLabel);
    lv_label_set_text(schema_walk_time_label, walkDurationLabel);
    lv_label_set_text_fmt(schema_repeat_value_label,"%d",schemaDef.repeat);
    Serial.printf("%d\n\r",runTimeSchema.currSectionIdx);
//    if(runTimeSchema.currSectionIdx>=0) {
//        lv_label_set_text(run_time_action_label,to_action(runTimeSchema.sections[runTimeSchema.currSectionIdx].action));
//    } else {
//        lv_label_set_text(run_time_action_label,"");
//    }
//    remaining_time(runTimeSchema);
//    lv_label_set_text(run_time_clock_label,remainingTimeLabel);
//    lv_label_set_text(run_time_action_btn, to_status_label(runTimeSchema.status));
}
/*
    ______________________________
    |        |     |       |     |
    | Run    |  -  | 00:00 |  +  | // run bar     \
    |________|_____|_______|_____|                 \
    |        |     |       |     |                  \
    | Walk   |  -  | 00:00 |  +  | // Walk bar       | Schema Bar
    |________|_____|_______|_____|                  /
    |        |     |       |     |                 /
    | Repeat |  -  |     0 |  +  | // Repeat bar  /
    |________|_____|_______|_____|
    |        |        |          |
    | action |  00:00 |   <btn>  | // run_time bar ; btn=Setup|Start|Pauze
    |________|________|__________|
    | Reset        |        Exit | // action bar
    |______________|_____________|

*/

void runcoach_app_gui_setup( uint32_t tile_num ) {
    runcoach_initialize_styles(tile_num);
    lv_obj_t * runcoach_app_container = wf_add_container( runcoach_app_main_tile, LV_LAYOUT_COLUMN_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false );
    schema_setup(runcoach_app_container);
    run_time_bar_setup(runcoach_app_container);
    actionbar_setup(runcoach_app_container);
   // runcoach_update_labels();
}