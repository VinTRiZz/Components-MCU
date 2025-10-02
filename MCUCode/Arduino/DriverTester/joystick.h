
#include <stdint.h>

void joystick_setup(const uint8_t pinX, const uint8_t pinY, const uint8_t pinButton, void (*buttonPressSignal)() );

void joystick_checkButton();

uint8_t joystick_posX(uint8_t & direction);

uint8_t joystick_posY(uint8_t & direction);