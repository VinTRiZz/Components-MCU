#include "compiler.h"
#include <avr/interrupt.h>
#include "conf_example.h"

// Only use Pin Change Interrupt handler for devices supporting this.
#ifdef EXAMPLE_PCINT_vect

/**
 * \brief Pin change interrupt handler for PB0
 *
 * Create breakpoint here and then run. Press SW0 in order to test
 * this.
 */
ISR(EXAMPLE_PCINT_vect)
{
	/* Toggle PD0 (LED0)
	 *
	 * Note that it will toggle both on button press and release.
	 */
	PIND = (1 << PIND0);
}

#endif

/**
 * \brief Main routine
 */
int main(void)
{
	// Variable to put switch input into
	uint8_t val;

	/* Use port D for output to control the STK600 LEDs.
	 *
	 * First set the data direction register to 0xff in order to set all
	 * port pins to output function.
	 */
	DDRD = 0xff;

	// Set output levels high. Will turn off STK600 LEDs.
	PORTD = 0xff;

	// Set output levels low. Will turn on STK600 LEDs.
	PORTD = 0;


	PORTB = 0xff;

	val = PINB;

while (1)
{

	/* It's also possible to control a single or a set of pins on a port.
	 *
	 * Setting pin PD0 high in order to turn off LED0.
	 */
	PORTD |= (1 << PORTD0);

	// Setting pin PD1 low in order to turn on LED1.
	PORTD &= ~(1 << PORTD0);
}
	
}
