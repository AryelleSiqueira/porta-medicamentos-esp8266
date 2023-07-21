#ifndef PILLBOXSETTINGS_H
#define PILLBOXSETTINGS_H

#include <Arduino.h>
#include <LinkedPointerList.h>
#include "Alarm.h"
#include "Slot.h"

#define PROGRAMMING 0
#define WAITING 1
#define ALARM 2
#define REFILL 3

typedef struct {
  Slot* slots[MAX_SLOTS];
  LinkedPointerList<Alarm> alarms;
  int slotsCount;
  String authToken;
  String code;
  int mode;
} PillboxSettings;

void programmingMode(PillboxSettings*);
void printAlarms(PillboxSettings*);
Alarm* getNextAlarm(PillboxSettings*);
int refillMode(PillboxSettings*);

#endif