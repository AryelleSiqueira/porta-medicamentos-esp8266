#include "Display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displaySetup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
}

void displayText(const String text[], const int textSize[], const int size) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  for (int i = 0; i < size; i++) {
    display.setTextSize(textSize[i]);
    display.println(text[i]);
  }
  display.display();
}