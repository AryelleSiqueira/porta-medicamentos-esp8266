#include "Alarm.h"

Alarm::Alarm(const int hour, const int min) : hour(hour), min(min), numSlots(0) {}

void Alarm::addSlot(const int slot) {
  if (numSlots < MAX_SLOTS) {
    slots[numSlots] = slot;
    numSlots++;
  }
}

void Alarm::printSlots() {
  for (int i = 0; i < numSlots; i++) {
    Serial.print(slots[i]);
    Serial.print(" ");
  }
  Serial.println();
}