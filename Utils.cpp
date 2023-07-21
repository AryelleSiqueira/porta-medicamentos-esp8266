#include <time.h>
#include "Utils.h"
#include "Display.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

void timeSetup() {
  configTime(0, -3600*3, "pool.ntp.org");
  while(!time(nullptr)){
    delay(500);
  }
}

bool isCurrentTime(Alarm& alarm) {
  time_t rawtime; 
  struct tm* timeinfo; 
  time(&rawtime);

  timeinfo = localtime(&rawtime);

  return alarm.getHour() == timeinfo->tm_hour && alarm.getMin() == timeinfo->tm_min;
}

void enableWifi() {
  Serial.println("Enabling WiFi...");

  WiFiManager wm;
  wm.autoConnect("Pillbox_AP");
}

void disableWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Disabling WiFi...");
    WiFi.mode(WIFI_OFF);
  }
}

void writeFile(String content, String fileName) { 
  File file = SPIFFS.open("/" + fileName, "w");

  if(!file){
    Serial.println("Erro ao abrir arquivo " + fileName);
  } else {
    file.println(content);
  }
  file.close();
}

String readFile(String fileName) {
  if (SPIFFS.exists("/" + fileName)) {
    File file = SPIFFS.open("/" + fileName, "r");

    if (!file) {
      Serial.println("Erro ao abrir arquivo " + fileName);
    } else {
      String content = file.readStringUntil('\r');
      file.close();
      return content;
    }
  }
  return "";
}

void printCode(String code) {
  String text[3] = {"Codigo:", "", code};
  int sizes[3] = {2, 1, 3};
  displayText(text, sizes, 3);
}

void printSlot(Slot& slot) {
  String text[] = {"Slot " + String(slot.getId()+1), "", slot.getDescription(), "", "[X] Prox->"};
  int sizes[] = {2, 1, 2, 1, 2};
  if (slot.getDescription().length() > 10) {
    sizes[2] = 1;
  }
  displayText(text, sizes, 5);
}

void printStop() {
  String text[] = {"", " Parar", "", "Aperte [X]"};
  int sizes[] = {1, 3, 2, 2};
  displayText(text, sizes, 4);
}

void printEnterProgrammingMode() {
  String text[] = {"Programar?", "", "   S [X]"};
  int sizes[] = {2, 2, 2};
  displayText(text, sizes, 3);
}

void printConfiguring() {
  String text[] = {"Conectando", "  com o", "servidor"};
  int sizes[] = {2, 2, 2};
  displayText(text, sizes, 3);
}

void printExitingProgrammingMode() {
  String text[] = {"Saindo", "  do modo", "PROGRAMAR"};
  int sizes[] = {2, 2, 2};
  displayText(text, sizes, 3);
}

void printExitingRefillMode() {
  String text[] = {"Saindo", "  do modo", "RECARGA"};
  int sizes[] = {2, 2, 2};
  displayText(text, sizes, 3);
}

void printRefill() {
  String text[] = {"Abastecer?", "", "   S [X]", "", "N [...5s]"};
  int sizes[] = {2, 1, 2, 1, 2};
  displayText(text, sizes, 5);
}

void printInit() {
  String text[] = {"", "Start"};
  int sizes[] = {1, 3};
  displayText(text, sizes, 2);
}

void printTime() {
  time_t rawtime; 
  struct tm* timeinfo; 
  time(&rawtime);

  timeinfo = localtime(&rawtime);

  Serial.println(String(timeinfo->tm_hour) + ":" + String(timeinfo->tm_min) + ":" + String(timeinfo->tm_sec));  
}