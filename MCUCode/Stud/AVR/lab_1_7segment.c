#include "compiler.h"
#include <avr/interrupt.h>
#include "conf_example.h"

const char dig[2] = {0b00000111, 0b00111111}; // Добавлено по заданию


#ifdef EXAMPLE_PCINT_vect
ISR(EXAMPLE_PCINT_vect)
{
	PIND = (1 << PIND0);
}
#endif


int main(void)
{
	
	DDRD = 0xff;
	PORTD = 0xff;
	
	DDRB = 0x0f; // Последние 4 бита для эффекта "опроса"
	const short int checker[4] = {0b0001, 0b0010, 0b0100, 0b1000}; // Массив "проверок"
	
	
	DDRC = 0xff; // Для индикатора
	unsigned char number[] =
	{
		0x3f, //0
		0x06, //1
		0x5b, //2
		0x4f, //3
		0x66, //4
		0x6d, //5
		0x7d, //6
		0x07, //7
		0x7f, //8
		0x6f //9
	};
	
	
	uint8_t i = 0; // Цикловая переменная
	uint8_t num = 0; // Для счётчика
	
while (1)
{
	PORTD = dig[0]; // Изменено по заданию
	asm("nop"); // Изменено по заданию
  
  /*
  // Для эффекта "ёлочки"
  if (PORTD == 255) // Обнуление
		PORTD = 0;
	
	if (PORTB > 127)
		PORTD = (PORTD << 1) + 1; // Ёлочка влево
	else
		PORTD = (PORTD >> 1) + 128; // Ёлочка вправо

  
  */
	
	
	// "Счётчик" на порту С
	if (num < 10)
		num++;
	else
		num = 0;
	PORTC = number[num];
	
	
	// Ниже - "опрос" на порту В
	if (i < 4)
		i++;
	else
		i = 0;
	PORTB = checker[i];
}
