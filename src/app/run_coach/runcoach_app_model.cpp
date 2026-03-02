#include "runcoach_app_model.h"
#include "lvgl.h"
#include <HardwareSerial.h>

SchemaDef schemaDef= { .runTime = 180, .walkTime = 180, .repeat = 5};
RunTimeSchema runTimeSchema = { .sections = nullptr, .status=STOPPED, .currSectionIdx=-1, .nrOfSections=0 };

/* Frees the memory allocated for the sections in the run schema. */
static void runcoach_freeRunSchemaSections() {
    if(runTimeSchema.sections == nullptr) {
        return;
    }
    for(int idx = 0; idx < runTimeSchema.nrOfSections; idx++) {
        if(runTimeSchema.sections[idx].actionLabel) {
            free(runTimeSchema.sections[idx].actionLabel);
            runTimeSchema.sections[idx].actionLabel = nullptr;
        }
    }
    delete[] runTimeSchema.sections;
    runTimeSchema.sections = nullptr;
    runTimeSchema.nrOfSections = 0;
}

static char* runcoach_buildActionLabel(const char* action, int currentRepeat, int totalRepeats) {
    char* label = (char*)malloc(24);
    if(label == nullptr) {
        return (char*)"";
    }
    snprintf(label, 24, "%s (%d/%d)", action, currentRepeat, totalRepeats);
    return label;
}

void runcoach_calculateRunSchema() {
    runcoach_freeRunSchemaSections();
    runTimeSchema.sections = new Section[schemaDef.repeat*2];
    runTimeSchema.nrOfSections = schemaDef.repeat * 2;
    for (int i=0;i<schemaDef.repeat;i++) {
        runTimeSchema.sections[i*2]= { .action=RUN, .actionLabel=runcoach_buildActionLabel("Run", i + 1, schemaDef.repeat), .duration= schemaDef.runTime};
        runTimeSchema.sections[i*2+1]= {.action=WALK, .actionLabel=runcoach_buildActionLabel("Walk", i + 1, schemaDef.repeat), .duration=schemaDef.walkTime};
    };
}

void runcoach_model_init() {
    schemaDef.runTime=180;
    schemaDef.walkTime=60;
    schemaDef.repeat=5;
    runTimeSchema.status=STOPPED;
    runTimeSchema.currSectionIdx=-1;
    runTimeSchema.currSectionStart=0;
    runTimeSchema.currSectionPauzedOn=0;
    runcoach_calculateRunSchema();
}

/* Calculates the run schema and launches the timer */
void runcoach_launch_schema() {
    Serial.println("IN runcoach_launch_schema");
    runcoach_calculateRunSchema();
    runTimeSchema.status=RUNNING;
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
