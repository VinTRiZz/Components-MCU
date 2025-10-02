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
	
	DDRD = 0xff;
	DDRB = 0xff; // Активация порта В на вывод
	PORTD = 0xff;
	PORTD = 0;
	PORTB = 0xff;
	val = PINB;
while (1)
{
	PORTB = 1; // start (port on)
	for (int i = 0; i < 8; i++)
		PORTB = PORTB << 1; // 0000 0001 -> 0000 0010
	PORTB = 0; // end (port off)
		
	PORTD = 128; // start (port on)
	for (int i = 0; i < 8; i++)
		PORTD = PORTD >> 1; // 1000 0000 -> 0100 0000
	PORTD = 0; // end (port off)
}
	
}
