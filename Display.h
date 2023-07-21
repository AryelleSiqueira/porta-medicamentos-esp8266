#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

void displaySetup();
void displayText(const String[], const int[], const int);

#endif