#include <FS.h>
#include <AccelStepper.h>
#include "Alarm.h"
#include "Slot.h"
#include "PillboxSettings.h"
#include "Utils.h"
#include "Display.h"
#include "BuzzerUtils.h"

#define BUTTON_PIN D0

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8 

const int steps_per_rev = 2048;

AccelStepper motor(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

PillboxSettings ps;

Alarm* next = NULL;
int countSlot = 0;
int buttonPressed = 0;
int alarmOn = 0;
int alarmStopped = 0;
int doRefill = 0;

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();

  pinMode(BUTTON_PIN, INPUT);
  pinMode(buzzerPin, OUTPUT);

  displaySetup();
  printInit();

  motor.setMaxSpeed(2000);
  motor.setAcceleration(400.0);

  enableWifi();
  timeSetup();
  disableWifi();
  delay(100);
  
  ps.mode = PROGRAMMING;
}

void loop() {
  if (!buttonPressed) {

    if (ps.mode == PROGRAMMING) {
      programmingMode(&ps);
      next = getNextAlarm(&ps);
      doRefill = refillMode(&ps);
      if (doRefill) {
        ps.mode = REFILL;
      } else {
        printEnterProgrammingMode();
        ps.mode = WAITING;
      }
    }

    else if (ps.mode == WAITING) {
      if (isCurrentTime(*next)) ps.mode = ALARM;
      if (digitalRead(BUTTON_PIN) != LOW) ps.mode =  PROGRAMMING;
    }

    else if (ps.mode == ALARM) {
      if (!alarmStopped) {
        playAlarm();
        printStop();
      } 
      if (digitalRead(BUTTON_PIN) != LOW) {
        alarmStopped++;
        buttonPressed++;

        if (countSlot < next->getNumSlots()) { 
          int atual = 0;
          if (countSlot != 0) {
            atual = next->getSlot(countSlot-1)+1;
          }
          const int slotIdx = next->getSlot(countSlot);
          motor.moveTo(motor.currentPosition() - (slotIdx+1-atual)*682);
          printSlot(*(ps.slots[slotIdx]));
          countSlot++;
        }
        else { 
          const int slotIdx = next->getSlot(countSlot-1);
          motor.moveTo(motor.currentPosition() - (MAX_SLOTS - slotIdx)*682);
          next = getNextAlarm(&ps);
          countSlot = 0;
          alarmStopped = 0;
          printEnterProgrammingMode();
          ps.mode = WAITING;
        }
      }
    }

    else if (ps.mode == REFILL) {
      buttonPressed++;

      if (countSlot == 0 || digitalRead(BUTTON_PIN) != LOW) {
        if (countSlot < MAX_SLOTS) {
          if (ps.slots[countSlot] == NULL) {
            Slot slot(countSlot++, "SEM_NOME");
            printSlot(slot);
          } else {
            printSlot(*(ps.slots[countSlot++]));
          }
        }
        else {
          countSlot = 0;
          printEnterProgrammingMode();
          ps.mode = WAITING;
        }
      }
      motor.moveTo(motor.currentPosition() - 682);
    }
  }
  motor.run();

  if (motor.distanceToGo() == 0) {
    buttonPressed = 0;
  }
  delay(10);
}