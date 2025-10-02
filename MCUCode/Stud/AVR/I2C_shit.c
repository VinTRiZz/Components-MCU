#include <avr/io.h>
#include <interrupt.h>
// ATmega16 -- master
// MS -- slave

// Data send instruction
int IIC_send(char data); // prototype
// Timer initializer
void tim0_init(void);

char data[] = {'a'}; // for send
int err_counter = 0; // counts errors in sending data
int err_exist = 0; // answers question "Has sending operation errors?"
int time = 0; // time

// MUST DEFINE WHAT TO USE
#define SIZE 1 // Size of data to send
#define ADDRES 0x5553 // the only addres found in doc

int main(void)
{
	// Initialization
	// PORTD 7 is DATA
	// PORTC 0 is CLOCK
	
	// output modes
	DDRD = 128; // 1000 0000
	DDRC = 1; // 0000 0001
	PORTD = 1; // set CLOCK to 1
	
	// CLOCK init
	tim0_init();
	
    while(1)
    {
		for (int i = 0; i < SIZE; i++)
			if ( !IIC_send(data[i]) ) // if there any error
				err_counter++;
		
		_delay_ms(1500); // delay before next send
    }
}




void tim0_init(void)
{
	sei(); // enadle interrupts for timer overflow
	TCCR0 = 0;
	
	TCCR0 |= (1<<CS02)| (1<<CS00); // prescale: 1024 (to 1953 Hz)
	// it means one tick in 513 us
	
	TCNT0 = 114; // 10 ticks, work every 5 ms
}

ISR(TIMER0_OVF_vect)
{
	PORTC = ~PORTC; // clock half-period
	
	time++;
	
	TCNT0 = 114; // 10 ticks, work every 5 ms
}



// Data send instruction
int IIC_send(char data)
{
	
	// PORTD 7 is DATA
	// PORTC 0 is CLOCK
	int bufTime = time;
	int ret = 0; // to return result of send
	
	//S START
	while (PORTC.0 != 1)
		asm("nop");
	PORTD = 1;
	//E START
	
	//S addr
	for (int i = 1; i < 8); i++)
	while ( (time - bufTime) < i)
	{
		PORTD = ADDRES >> (i-1);
	}
	//E addr
	
	//S send
	for (int i = 1; i < (sizeof(data)); i++)
		while ( (time - bufTime) < i)
		{
			PORTD = data >> (i-1);
		}
	//E send
	
	//S check
	DDRD = 0; // check answer from MS
	bufTime = time;
	while ( (bufTime - time) != 1)
		ret = !PIND;
	//E check
	
	//S START
	while (PORTC.0 != 1)
		asm("nop");
	PORTD = 0;
	//E START
	
	return ret; // send result to main function
	
	
	
	

	/*
	FIRST VERSION
	// Request adress
	for (int i == 0; i < 8; i++)
		PORTD = (data >> i) & 1;
	
	// Start send
	PORTD = 0b01; // 1 bit is CLOCK, 0 bit is DATA
	
	// Send data
	for (short int i = 0; i < 8; i++)
		PORTD &= ;
	
	DDRD = 0b10; // Set 0 pin of D to receive signal
	
	// Check for errors and return from func
	if (PORTD0 == 0)
	{
		// End sending
		PORTD = 0b11; // 1 bit is CLOCK, 0 bit is DATA
		return 1;
	}
	else
	{
		// End sending
		PORTD = 0b11; // 1 bit is CLOCK, 0 bit is DATA
		return 0;
	}
	*/
}
