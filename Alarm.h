#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

#include "Slot.h"

const int MAX_SLOTS = 5; 

class Alarm {
public:
  Alarm(const int hour, const int min);

  void addSlot(const int slot);
  void printSlots();
  int getHour() { return hour; }
  int getMin() { return min; }
  int getSlot(int i) { return slots[i]; }
  int getNumSlots() { return numSlots; }

private:
  int hour;
  int min;
  int slots[MAX_SLOTS];
  int numSlots;
};

#endif