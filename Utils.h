#ifndef UTILS_H
#define UTILS_H

#include <FS.h>
#include <Arduino.h>
#include "Alarm.h"

void timeSetup();
bool isCurrentTime(Alarm&);
void enableWifi();
void disableWifi();
String readFile(String);
void writeFile(String, String);
void printTime();
void printSlot(Slot&);
void printCode(String);
void printStop();
void printRefill();
void printConfiguring();
void printExitingProgrammingMode();
void printExitingRefillMode();
void printEnterProgrammingMode();
void printInit();

#endif