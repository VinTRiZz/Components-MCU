#include "joystick.h"

static void (*joystick_buttonPressSignal)();

static uint8_t joystick_pinX, joystick_pinY, joystick_pinButton;

#ifdef ARDUINO

#include <Arduino.h>
#define READ_PIN(a) digitalRead(a)
#define READ_PIN_ANALOG(a) analogRead(a)

#define ADC_COEFF 5.0 / 1024.0

#else

#include <stm32fxx_hal.h>

#define ADC_CHANNEL_COUNT 1
uint16_t adc[ADC_CHANNEL_COUNT] {};

#define PORT B
#define READ_PIN(a) HAL_GPIO_ReadPin(GPIO##PORT, a)
#define READ_PIN_ANALOG(a) adc[a]

#define ADC_COEFF 3.3 / 1024.0

#endif // ARDUINO

void joystick_setup(const uint8_t pinX, const uint8_t pinY, const uint8_t pinButton, void (*buttonPressSignal)() )
{
  joystick_buttonPressSignal = buttonPressSignal,
  joystick_pinX = pinX, 
  joystick_pinY = pinY,
  joystick_pinButton = pinButton;
}

void joystick_checkButton()
{
  if (READ_PIN(joystick_pinButton))
  {
    joystick_buttonPressSignal();
  }
}

inline uint8_t joystick_posX()
{
  return READ_PIN_ANALOG(joystick_pinX) * ADC_COEFF;
}

inline uint8_t joystick_posY()
{
  return READ_PIN_ANALOG(joystick_pinY) * ADC_COEFF;
}