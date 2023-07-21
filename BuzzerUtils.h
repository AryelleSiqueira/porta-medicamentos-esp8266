#ifndef BUZZER_UTILS_H
#define BUZZER_UTILS_H

#include <Arduino.h>

const int melody[] = {
  261,  // C4
  293,  // D4
  329,  // E4
  349,  // F4
  349,  // F4
  349,  // F4
};

const int buzzerPin = D1;

void playAlarm();

#endif