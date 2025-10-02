#define F_CPU 2000000UL // frequency

#include <avr/io.h>
#include <avr/interrupt.h>

// TIMER FREQ == 44.1 kHz
// get ADC while not interrupted


#define Vref 2.56 // Vref in ADC convert formula

short int ADC_RESULT = 0; // result of ADC convert
short int times = 0;


// Init in one conv mode, while timer
// not get number, will convert.
// Otherwise will stop immediately
void ADC_init(void)
{
	// for safe
	ADCSRA = 0;
	ADMUX = 0;
	SFIOR = 0;
	
	//MODE: Enabled start when Timer/counter0 overflow
	SFIOR &= ~(1 << ADTS2);
	SFIOR |= (1 << ADTS0);
	SFIOR |= (1 << ADTS1);
	
	// MODE: SFIOR enabled; ADC enabled
	// Ones
	ADCSRA |= (1 << ADATE) | (1<<ADEN);
	// Zeros
	ADCSRA &= ~(1 << ADPS2);
	ADCSRA &= ~(1 << ADPS1);
	ADCSRA &= ~(1 << ADPS0);
	ADCSRA &= ~(1 << ADIF);
	ADCSRA &= ~(1 << ADIE);
	
	// MODE: Vref = 2.56 V; RAR; ADC1 for work
	// Ones
	ADMUX |= (1 << REFS0) | (1 << REFS1) | (1 << MUX0);
	// Zeros
	ADMUX &= ~(0<<ADLAR);
	ADMUX &= ~(0 << MUX1);
	ADMUX &= ~(0 << MUX2);
	ADMUX &= ~(0 << MUX3); 
	ADMUX &= ~(0 << MUX4);
}

void TIMER0_init(void)
{
	// Need 44.1 kHz
	
	// No prescaler, so because
	// (2*10^6) / 44100 == 45.35
	// every 45 iteration will start ADC and collect
	// in next 45.
	// 0.3 will collect, so every 135 iteration (3 times)
	// timer will be increased by 1 more.
	
	
	// for safe
	TCCR0 = 0;
	
	// MODE: normal; clear OC0 if match;
	// Ones
	TCCR0 |= (1 << COM01) | (1 << CS00);
	// Zeros
	TCCR0 &= ~(1 << WGM00);
	TCCR0 &= ~(1 << WGM01);
	TCCR0 &= ~(1 << COM00);
	TCCR0 &= ~(1 << CS01);
	TCCR0 &= ~(1 << CS02);
	
	// To compare, means interval
	OCR0 = 45;
	
}


// Compares to stop ADC convert
ISR(TIMER0_COMP_vect)
{
	// every 3 iteration increase by 1
	if (times == 2)
	{
		times = 0;
		OCR0 = 46;
	}
	else
		OCR0 = 45;
		
	// Get result value as a voltage
	if (ADSC == 0)
		ADC_RESULT = ( ADC * Vref ) / 1024;
}






int main()
{
	sei(); // alloy interrupts
	
	ADC_init(); // initialise ADC
	
	TIMER0_init(); // initialise timer0
	
	
	
	while(1)
	{
		// It will start conversation
		// after initialise.
		// After every 45 ticks of timer0
		// it will stop and remember
		// the result in 
		// ADC_RESULT
		asm("nop");
	}
	
	return 0;
}
