#include "BuzzerUtils.h"

void playAlarm() {
  for (int i = 0; i < 6; i++) {
    tone(buzzerPin, melody[i], 200);
    if (digitalRead(D0) != LOW) break;
    delay(250);
  }
  noTone(buzzerPin);
}