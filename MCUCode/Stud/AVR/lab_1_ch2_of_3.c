
#include "compiler.h"
#include <avr/interrupt.h>
#include "conf_example.h"


#ifdef EXAMPLE_PCINT_vect
ISR(EXAMPLE_PCINT_vect)
{
	PIND = (1 << PIND0);
}
#endif


int main(void)
{
	uint8_t val;
	uint8_t i; // Добавление переменной i по заданию
	DDRD = 0xff;
	PORTD = 0xff;
	PORTD = 0;
	PORTB = 0xff;
	val = PINB;
while (1)
{

	PORTD |= (1 << i);
	i++;
	if (i>8)
	{
		i=0;
		PORTD =0;
	}
}	
}
