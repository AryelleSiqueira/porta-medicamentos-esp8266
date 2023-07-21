#ifndef SLOT_H
#define SLOT_H

#include <Arduino.h>

class Slot {
public:
  Slot(const int id, const String& description);

  int getId() { return id; }
  String getDescription() { return description; }

private:
  int id;
  String description;
};

#endif