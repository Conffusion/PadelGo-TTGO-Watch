#include "runcoach_app_model.h"
#include "lvgl.h"
#include <HardwareSerial.h>

SchemaDef schemaDef= { .runTime = 180, .walkTime = 180, .repeat = 5};
RunTimeSchema runTimeSchema = { .sections = nullptr, .status=STOPPED, .currSectionIdx=-1 };

void calculateRunSchema() {
    runTimeSchema.sections = new Section[schemaDef.repeat*2];
    for (int i=0;i<schemaDef.repeat;i++) {
        runTimeSchema.sections[i*2]= { .action=RUN, .duration= schemaDef.runTime};
        runTimeSchema.sections[i*2+1]= {.action=WALK, .duration=schemaDef.walkTime};
    };
}

void runcoach_model_init() {
    schemaDef.runTime=180;
    schemaDef.walkTime=60;
    schemaDef.repeat=5;
    calculateRunSchema();
}

/* Calculates the run schema and launches the timer */
void runcoach_launch_schema() {
    Serial.println("IN runcoach_launch_schema");
    calculateRunSchema();
    runTimeSchema.currSectionIdx=0;
    runTimeSchema.currSectionStart=time(0);
}

void runcoach_pauze_schema() {
    runTimeSchema.status=PAUZING;
    runTimeSchema.currSectionPauzedOn=time(0);
}

/* status changed from PAUZING to RUNNING */
void runcoach_continue_schema() {
    // increase the start time with the number of pauzed seconds
    double pauzed_seconds = difftime(time(0),runTimeSchema.currSectionPauzedOn);
    runTimeSchema.currSectionStart=runTimeSchema.currSectionStart+pauzed_seconds;
    runTimeSchema.status=RUNNING;
}
