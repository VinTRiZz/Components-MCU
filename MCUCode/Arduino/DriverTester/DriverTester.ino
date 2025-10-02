#include "joystick.h"

#include <Wire.h>
#include <GyverOLED.h>

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
//GyverOLED<SSH1106_128x64> oled;

volatile uint8_t posX = 0;
volatile uint8_t posY = 0;

void setup() 
{
  joystick_setup(3, 4, 7, showSignal);
  oled.init();
}

void loop() 
{
  joystick_checkButton();
  delay(500);
}

void showSignal()
{
  oled.clear();
  oled.print("Test");
  oled.update();
}
