#include "runcoach_app_model.h"
#include "lvgl.h"
#include <HardwareSerial.h>

SchemaDef schemaDef= { .runTime = 180, .walkTime = 180, .repeat = 5};
RunTimeSchema runTimeSchema = { .sections = nullptr, .status=STOPPED, .currSectionIdx=-1, .nrOfSections=0 };

static char* runcoach_buildActionLabel(const char* action, int currentRepeat, int totalRepeats) {
    char* label = (char*)malloc(24);
    if(label == nullptr) {
        return nullptr;
    }
    snprintf(label, 24, "%s (%d/%d)", action, currentRepeat, totalRepeats);
    return label;
}


void runcoach_appendRunSchema() {
    int additionalSections = schemaDef.repeat * 2;
    if(additionalSections <= 0) {
        return;
    }

    short oldCount = runTimeSchema.nrOfSections;
    short newCount = oldCount + additionalSections;
    Section* oldSections = runTimeSchema.sections;
    Section* newSections = new Section[newCount];

    for(short idx = 0; idx < oldCount; idx++) {
        newSections[idx].action = oldSections[idx].action;
        newSections[idx].duration = oldSections[idx].duration;
        newSections[idx].actionLabel = nullptr;
    }

    for(int i = 0; i < schemaDef.repeat; i++) {
        short baseIdx = oldCount + i * 2;
        newSections[baseIdx].action = RUN;
        newSections[baseIdx].duration = schemaDef.runTime;
        newSections[baseIdx].actionLabel = nullptr;

        newSections[baseIdx + 1].action = WALK;
        newSections[baseIdx + 1].duration = schemaDef.walkTime;
        newSections[baseIdx + 1].actionLabel = nullptr;
    }

    int totalRepeats = newCount / 2;
    for(short idx = 0; idx < newCount; idx++) {
        const char* actionName = (newSections[idx].action == RUN) ? "Run" : "Walk";
        int repeatNumber = (idx / 2) + 1;
        newSections[idx].actionLabel = runcoach_buildActionLabel(actionName, repeatNumber, totalRepeats);
    }

    if(oldSections != nullptr) {
        for(short idx = 0; idx < oldCount; idx++) {
            if(oldSections[idx].actionLabel) {
                free(oldSections[idx].actionLabel);
                oldSections[idx].actionLabel = nullptr;
            }
        }
        delete[] oldSections;
    }

    runTimeSchema.sections = newSections;
    runTimeSchema.nrOfSections = newCount;
}

void runcoach_freeRunSchemaSections() {
    if(runTimeSchema.sections != nullptr) {
        for(short idx = 0; idx < runTimeSchema.nrOfSections; idx++) {
            if(runTimeSchema.sections[idx].actionLabel) {
                free(runTimeSchema.sections[idx].actionLabel);
                runTimeSchema.sections[idx].actionLabel = nullptr;
            }
        }
        delete[] runTimeSchema.sections;
        runTimeSchema.sections = nullptr;
        runTimeSchema.nrOfSections = 0;
    }
}

void runcoach_model_init() {
    schemaDef.runTime=180;
    schemaDef.walkTime=60;
    schemaDef.repeat=5;
    runTimeSchema.status=STOPPED;
    runTimeSchema.currSectionIdx=-1;
    runTimeSchema.currSectionStart=0;
    runTimeSchema.currSectionPauzedOn=0;
    runcoach_freeRunSchemaSections();
}

/* Calculates the run schema and launches the timer */
void runcoach_launch_schema() {
    Serial.println("IN runcoach_launch_schema");
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
