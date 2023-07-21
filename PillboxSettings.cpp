#include "PillboxSettings.h"
#include "Alarm.h"
#include "Slot.h"
#include "Utils.h"
#include <time.h>
#include "Display.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const String API_URL = "https://smart-pillbox-3609ac92dfe8.herokuapp.com";

std::unique_ptr<BearSSL::WiFiClientSecure> getClient() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  return client;
}

Alarm* getNextAlarm(PillboxSettings* ps) { // TODO: descobrir por que não funciona
  time_t rawtime; 
  struct tm* timeinfo; 
  time(&rawtime);

  timeinfo = localtime(&rawtime);

  for (int i = 0; i < ps->alarms.size(); i++) {
    Alarm* alarm = ps->alarms.get(i);
    Serial.println(alarm->getHour() + ":" + alarm->getMin());
    if (alarm->getHour() > timeinfo->tm_hour || (alarm->getHour() == timeinfo->tm_hour && alarm->getMin() > timeinfo->tm_min)) {
      return alarm;
    }
  }
  return ps->alarms.get(0);
}

int generateAuthCode() {
  HTTPClient http;

  http.begin(*getClient(), API_URL + "/code/get");
  int statusCode = http.GET();

  if (statusCode != 200) {
    Serial.println("error " + http.getString());
    return -1;
  }
  DynamicJsonDocument doc(40);
  deserializeJson(doc, http.getString());

  Serial.println("Code: " + String(doc["code"]));
  
  return doc["code"];
}

String getTokenForCode(String code) {
  HTTPClient http;
  http.begin(*getClient(), API_URL + "/code/get/token");
  http.addHeader("Content-Type", "application/json");

  const int responseCode = http.POST("{\"code\": " + code + "}");

  Serial.println(responseCode);

  if (responseCode == 200) {
    DynamicJsonDocument doc(300);
    deserializeJson(doc, http.getString());
    
    return doc["token"];
  }
  return "";
}

void clearConfig(PillboxSettings* ps) {
  ps->alarms.clear();
  for (int i = 0; i < ps->slotsCount; i++) {
    delete ps->slots[i];
  }
  ps->slotsCount = 0;
}

void parseConfig(PillboxSettings* ps, String json) {
  clearConfig(ps);

  DynamicJsonDocument doc(3000);
  deserializeJson(doc, json);

  for (int i = 0; i < MAX_SLOTS; i++) {
    String slotName = "S" + String(i + 1);
    String slotMed = doc["slots"][slotName];
    ps->slots[i] = new Slot(i, slotMed);
    ps->slotsCount++;
  }
  for (int i = 0; i < doc["alarms"].size(); i++) {
    int hour = doc["alarms"][i]["hour"];
    int min = doc["alarms"][i]["minute"];
    Alarm* alarm = new Alarm(hour, min);

    for (int j = 0; j < doc["alarms"][i]["slots"].size(); j++) {
      const String slotName = doc["alarms"][i]["slots"][j];
      const int slotIdx = String(slotName.charAt(1)).toInt() - 1;
      alarm->addSlot(slotIdx);
    }
    ps->alarms.add(alarm);
  }
}

void retrieveCode(PillboxSettings* ps) {
  const String codeStr = readFile("code.txt");

  if (codeStr != NULL && codeStr != "") {
    ps->code = codeStr;
    return;
  }
  while (true) {
    int code = generateAuthCode();

    if (code < 0) {
      Serial.println("Error getting code. Retrying...");
      delay(1000);
      continue;
    } else {
      ps->code = String(code);
      writeFile(ps->code, "code.txt");
    }
  }
}

int authSetup(PillboxSettings* ps) {
  if (ps->code == "" || ps->code == NULL) {
    retrieveCode(ps);
  }
  for (int i = 0; i < 150; i++) {
    Serial.print(".");
    const String token = getTokenForCode(ps->code);

    if (token != "") {
      ps->authToken = token;
      Serial.println("Bearer token retrieved.");
      return 1;
    }
    printCode(ps->code);
    delay(5000);
  }
  return 0;
}

int getConfigFromServer(PillboxSettings* ps) {
  HTTPClient http;

  http.begin(*getClient(), String(API_URL) + "/config");
  http.addHeader("Authorization", "Bearer " + ps->authToken);

  const int code = http.GET();

  if (code == 200) {
    String config = http.getString();
    if (config == "{\"slots\":null,\"alarms\":null}") {
      Serial.println("No config found.");
      return 0;
    }
    parseConfig(ps, config);
    Serial.println("Config retrieved from server.");
    return 1;
  }
  if (code == 401) {
    return -1;
  }
  return 0;
}

void programmingMode(PillboxSettings* ps) {
  printConfiguring();
  if (WiFi.status() != WL_CONNECTED) {
    enableWifi();
  }
  if (authSetup(ps)) {
    int result;
    for (int i = 0; i < 150; i++) {
      result = getConfigFromServer(ps);

      if (result == 1) {
        Serial.println("Config retrieved from server.");
        delay(1000);
        break;
      }
      if (result == -1) {
        if (!authSetup(ps)) {
          break;
        }
        i = 0;
      }
      Serial.println("No config found. Waiting for config...");
      delay(5000);
    }
  }
  disableWifi();
  printExitingProgrammingMode();
  Serial.println("Exiting programming mode.");
  delay(3000);
}

int refillMode(PillboxSettings* ps) {
  printRefill();

  int buttonPressed = 0;
  for (int i = 0; i < 50; i++) {
    if (digitalRead(D0) != LOW) {
      buttonPressed++;
      break;
    }
    delay(100);
  }
  if (buttonPressed) {
    return 1;
  }
  printExitingRefillMode();
  delay(2000);
  return 0;
}

void printAlarms(PillboxSettings* ps) { 
  for (int i = 0; i < ps->alarms.size(); i++) {
      Alarm* alarm = ps->alarms.get(i);
      Serial.print(alarm->getHour());
      Serial.print(":");
      Serial.print(alarm->getMin());
      Serial.print(" ");
      alarm->printSlots();
  }
}