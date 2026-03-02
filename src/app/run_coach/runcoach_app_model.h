#ifndef _RUNCOACH_APP_MODEL_H
    #define _RUNCOACH_APP_MODEL_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <cstdio>
#include <ctime>

typedef struct {
  /* Time to run in one section expressed in seconds. */
  short runTime; 
  /* Time to walk in one section expressed in seconds. */
  short walkTime;
  /* Number of times to repeat the run+walk sections */
  short repeat;
} SchemaDef;

/* Possible types of action for a section */
typedef enum {
  RUN = 0,
  WALK = 1
} Action;

typedef enum {
    STOPPED=0,
    RUNNING=1,
    PAUZING=2
} RunTimeStatus;

/* A section describes an action to perform for a certain period. */
typedef struct  {
  Action action;
  char* actionLabel;
  short duration;
} Section;

/* A RunSchema contains a sequence of Section's to perform. */
typedef struct {
  Section* sections;
  RunTimeStatus status;
  short currSectionIdx;
  short nrOfSections;
  /* Remaining time in the current section expressed in seconds. */
  short remainingTime;

  /* The time the current section started to calculate remaining time. */
  time_t currSectionStart;
  /* The time the current section was pauzed. */
  time_t currSectionPauzedOn;
} RunTimeSchema;

extern SchemaDef schemaDef;
extern RunTimeSchema runTimeSchema;


void to_time_label(short seconds, char* dest);

inline const char* to_status_label(RunTimeStatus status) {
    switch(status) {
        case RUNNING: return "Pauze";
        case PAUZING: return "Continue";
        default:      return "Start";
    }
}
inline const char* to_action(Action action) {
    switch(action) {
        case RUN: return "Run";
        case WALK: return "Walk";
        default: return "";
    }
}


void runcoach_model_init();
void runcoach_launch_schema();
void runcoach_pauze_schema();
void runcoach_continue_schema();

#endif // _RUNCOACH_APP_MODEL_H