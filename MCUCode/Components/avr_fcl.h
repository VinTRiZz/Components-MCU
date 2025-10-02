#ifndef AVR_FAST_CONFIG_H
#define AVR_FAST_CONFIG_H
/*
* Это файл с настройкой элементов АТмега для упрощения создания кода в будущем
*/
// Includes
#include <asf.h>
#include <interrupt.h>
#include <stdint.h>

// Not reallly needed
#define PIN_RESET 0
#define PIN_SET 1


// ******************** delay.h ****************
#include <util/delay.h>
#define F_CPU 8000000UL
// ******************** ******* ****************


// ********************************************************************************************
// ******************************             ADC                ******************************
// ********************************************************************************************
void init_ADC();
#define VREF 5 // Reference voltage

ISR(ADC_vect) // ADC_
{
	// ADC = Vin * 1024 / Vref
	int adc_result = ADC;
	// adc_result =  512*ADCH + ADCL;
	
	int voltage = (float)adc_result * (float)VREF / 1024.0;
	
}


// ********************************************************************************************
// ******************************           TIMERS               ******************************
// ********************************************************************************************
#define PWM1_pulse OCR1A // Easier
#define Timer0_Data TCNT0 // Current number in timer 0

void init_timers();

ISR(TIMER1_OVF_vect)
{
	Timer0_Data = 255; // Preset
}


// ********************************************************************************************
// ******************************             SPI                ******************************
// ********************************************************************************************
// Pins of SPI
#define SPI_PORT PORTB
#define SPI_MOSI_PIN 5
#define SPI_MISO_PIN 4
#define SPI_SCK_PIN 3
#define SPI_SS_PIN 2

// Set pins of SPI
#define SPI_MOSI_STATE ( SPI_PORT & ( PIN_SET << SPI_MOSI_PIN ) )
#define SPI_MISO_STATE ( SPI_PORT & ( PIN_SET << SPI_MISO_PIN ) )
#define SPI_SCK_STATE ( SPI_PORT & ( PIN_SET << SPI_SCK_PIN ) )
#define SPI_SS_STATE ( SPI_PORT & ( PIN_SET << SPI_SS_PIN ) )

#define SPI_SELF_ADDRESS 0xFF // Controller address in SPI net
void SPI_init(int8_t mode, int8_t prescale, int8_t spi_en_interrupts, int8_t transmit_order, int8_t master);
int8_t SPI_transfer(int8_t WORD, int8_t TIMEOUT);
int8_t SPI_SendSentence(int8_t * WORD_ARRAY, int LENGTH, int8_t ADDRESS, int8_t ONE_MSG_TIMEOUT);
int8_t SPI_read(int32_t TIMEOUT);


// ********************************************************************************************
// ******************************             I2C                ******************************
// ********************************************************************************************
void I2C_init();
void I2C_start();
void I2C_write(char Data);
int8_t I2C_read();
void I2C_stop();

// ********************************************************************************************
// ******************************            MAIN                ******************************
// ********************************************************************************************
int main()
{
	
	init_timers();
	
	init_ADC();
	
	while(1)
	{
		
		
		
	}
}


// ********************************************************************************************
// ******************************             ADC                ******************************
// ********************************************************************************************
void init_ADC()
{
	// Presets
	ADMUX = ADCSRA = ADCSRB = 0;
	
	// *************** ADMUX -- ADC Multiplexer Selection Register ***************************
	/* REFS0, REFS1 -- reference selection
	* 0 0 -- AREF
	* 0 1 -- AVcc, capacitor on AREF
	* 1 1 -- 1,1 V internal
	*/
	/*-----------------------------
	* ADLAR
	* if 0: ADCH = - - - -  - - 9 8; ADCL = 7 6 5 4  3 2 1 0
	* if 1: ADCH = 9 8 7 6  5 4 3 2; ADCL = 1 0 - -  - - - -
	*/
	/*-----------------------------
	* MUX3, MUX2, MUX1, MUX0 --- channel select, in 2 system
	*/
	// *************** ADCSRA -- – ADC Control and Status Register A **************************
	/*-----------------------------
	* ADEN -- ADC enable
	*/
	/*-----------------------------
	* ADSC -- Start conversion
	* Starts conversion first or one when written to 1
	*/
	/*-----------------------------
	* ADATE -- Auto trigger enable
	* If 1, ADC will start conversion on positive trigger signal
	*/
	/*-----------------------------
	* ADIF -- Interrupt flag (reserved, 1 when interr)
	*/
	/*-----------------------------
	* ADEN -- Interrupt enable
	*/
	/*-----------------------------
	* ADPS0, ADPS1 -- Prescaler select bits (pow of 2 in 2 system, zeros are 2)
	*/
	// ***************** ADCSRB – ADC Control and Status Register B ***************************
	/*-----------------------------
	* ADTS0, ADTS1... -- Auto trigger source
	* 0 0 0 -- Free running mode
	* 0 0 1 -- Analog comparator
	* 0 1 0 -- Analog interrupt request 0
	* 0 1 1 -- Timer/counter 0 compare A
	* 1 0 0 -- Timer/counter 0 overflow
	* 1 0 1, 1 1 0 -- Timer/counter 1 compare B and overflow
	* 1 1 1 -- Timer/counter 1 capture event
	*/
	// *******************DIDR0 – Digital Input Disable Register 0 *****************************
	/*-----------------------------
	* Disables ADC input for battery saving
	*/
}


// ********************************************************************************************
// ******************************           TIMERS               ******************************
// ********************************************************************************************
void init_timers()
{
	// PWMs
	// Phase correct -- Треугольник равносторонний
	// Freq(pwm) = Freq(clk) / (N * 256) // N -- prescaler
	
	// Fast PWM -- Пила? / Нет, я топор
	// Freq(pwm) = Freq(clk) / (N * 510) // N -- prescaler
	
	// *******************  TCCR0A – Timer/Counter Control Register A *****************************
	/*-----------------------------
	* COM0A1, COM0A0 -- Compare output mode
	* 0 0 Normal port operation, OC0A disconnected.
	* 0 1 Toggle OC0A on compare match
	* 1 0 Clear OC0A on compare match
	* 1 1 Set OC0A on compare match
	************
	* Compare Output Mode, Fast PWM Mode(1)
	* COM0A1, COM0A0 Description
	* 0 0 Normal port operation, OC0A disconnected.
	* 0 1 WGM02 = 0: Normal port operation, OC0A disconnected.
	*	  WGM02 = 1: Toggle OC0A on compare match.
	* 1 0 Clear OC0A on compare match, set OC0A at BOTTOM (non-inverting mode).
	* 1 1 Set OC0A on compare match, clear OC0A at BOTTOM (inverting mode).
	************
	* 0 0 Normal port operation, OC0A disconnected.
	* 0 1 WGM02 = 0: Normal port operation, OC0A disconnected.
		  WGM02 = 1: Toggle OC0A on compare match.
	* 1 0 Clear OC0A on compare match when up-counting. Set OC0A on compare match when down-counting.
	* 1 1 Set OC0A on compare match when up-counting. Clear OC0A on compare match when down-counting.
	*/
	/*-----------------------------
	* FOR OCR1B EVERYTHING IS EQUAL
	*/
	/*-----------------------------
	* WGM01, WGM00 -- Waveform generation mode. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WGM02 is at B register
	Num	 02 01 00	Mode				TOP		OCRx Updte	TOV Flag Set on(1)(2)
	0	 0 0 0		Normal				0xFF	Immediate	MAX
	1	 0 0 1		PWM,phase correct	0xFF	TOP			BOTTOM
	2	 0 1 0		CTC					OCRA	Immediate	MAX
	3	 0 1 1		Fast PWM			0xFF	BOTTOM		MAX
	4	 1 0 0		Reserved			–		–			–
	5	 1 0 1		PWM, phase correct	OCRA	TOP			BOTTOM
	6	 1 1 0		Reserved			–		– –
	7	 1 1 1		Fast				PWM		OCRA		BOTTOM TOP
	*/
	
	// *******************  TCCR0B – Timer/Counter Control Register B *****************************
	/*-----------------------------
	* FOC0A, FOC0B // NON-PWM
	* in PWM must be zero
	* Forces comparison with OC0A, OC0B
	*/
	/*-----------------------------
	* WGM02 --- wave form generation mode, see register A
	*/
	/*-----------------------------
	* CS02, CS01, CS00 -- Clock source prescaler
	* 0 0 0 No clock source (Timer/Counter stopped)
	* 0 0 1 clkI/O/(no prescaling)
	* 0 1 0 clkI/O/8 (from prescaler)
	* 0 1 1 clkI/O/64 (from prescaler)
	* 1 0 0 clkI/O/256 (from prescaler)
	* 1 0 1 clkI/O/1024 (from prescaler)
	* 1 1 0 External clock source on T0 pin. Clock on falling edge.
	* 1 1 1 External clock source on T0 pin. Clock on rising edge
	*/
	
	// Data:
	// TCNT0 – Timer/Counter Register
	
	// Pulse:
	// OCR0A – Output Compare Register A
	// OCR0B – Output Compare Register B
	
	// *******************  TIMSK0 – Timer/Counter Interrupt Mask Register *****************************
	// OCIE0A, OCIE0B: Timer/Counter Output Compare Match A,B Interrupt Enable
	// TOV0: Timer/Counter0 Overflow Flag ---- Sets when overflow
}






// ********************************************************************************************
// ******************************             SPI                ******************************
// ********************************************************************************************
// Data modes
/*

*/
/**
* @brief Inits SPI in MODE
* @param prescale Divides oscillator frequency by 2 in degree this value, up to 7
* @param spi_en_interrupts Enables interrupt if 1, disable if 0
* @param transmit_order If 1, starts from lower bit to upper. If 0, backward
* @param master If 1, this device is master. If 0, slave
* @note SPI Mode	 Conditions			Leading Edge		Trailing eDge
*		   0	CPOL=0, CPHA=0 Sample	(rising)			Setup (falling)
*		   1	CPOL=0, CPHA=1 Setup	(rising)			Sample (falling)
*		   2	CPOL=1, CPHA=0 Sample	(falling)			Setup (rising)
*		   3	CPOL=1, CPHA=1 Setup	(falling)			Sample (rising)
*		Other -- Do nothing
*/
void SPI_init(int8_t mode,int8_t prescale, int8_t spi_en_interrupts, int8_t transmit_order, int8_t master)
{
	SPCR = SPSR = 0;
	
	// Enable
	SPCR |= (1<<SPE);
	
	// Define mode of transmit
	switch (mode)
	{
		case 0:
			// 0 is default
		break;
		case 1:
			SPCR |= (1<<CPHA);
		break;
		case 2:
			SPCR |= (1<<CPOL);
		break;
		case 3:
			SPCR |= (1<<CPOL) | (1<<CPHA);
		break;
	}
	
	// Define frequency
	switch (prescale)
	{
		case 1:
			SPCR |= (1<<SPR1) | (1<<SPR0); // 128
			SPSR |= (0<<SPI2X);
		break;
		case 2:
			SPCR |= (1<<SPR1) | (0<<SPR0); // 64
			SPSR |= (0<<SPI2X);
		break;
		case 3:
			SPCR |= (1<<SPR1) | (0<<SPR0); // 32
			SPSR |= (1<<SPI2X);
		break;
		case 4:
			SPCR |= (0<<SPR1) | (1<<SPR0); // 16
			SPSR |= (0<<SPI2X);
		break;
		case 5:
			SPCR |= (0<<SPR1) | (1<<SPR0); // 8
			SPSR |= (1<<SPI2X);
		break;
		case 6:
			SPCR |= (0<<SPR1) | (0<<SPR0); // 4
			SPSR |= (0<<SPI2X);
		break;
		case 7:
			SPCR |= (0<<SPR1) | (0<<SPR0); // 2
			SPSR |= (1<<SPI2X);
		break;
	}
	
	// Enable interrupts
	if (spi_en_interrupts)
		SPCR |= (1<<SPIE);
		
	// Define transmit order
	if (transmit_order)
		SPCR |= (1<<DORD); 
	
	// Select master or slave mode
	if (master)
		SPCR |= (1<<MSTR);
}

/**
* @brief Sends word by SPI, don't send address before
* 
*/
int8_t SPI_transfer(int8_t WORD, int8_t TIMEOUT)
{
	// ********************  SPDR – SPI Data Register ******************************
	// Write data to transfer. MSB is 7 bit, LSB is 0 bit.
	SPDR = WORD;
	for (int i = 0; !(SPSR & (SPIF << 1)); i++)
		if (i <= TIMEOUT)
			_delay_us(1);
		else
			return -1;
}

int8_t SPI_SendSentence(int8_t * WORD_ARRAY, int LENGTH, int8_t ADDRESS, int8_t ONE_MSG_TIMEOUT)
{
	SPI_transfer(ADDRESS, ONE_MSG_TIMEOUT);
	
	for (int i = 0; i < LENGTH; i++)
		if (SPI_transfer(WORD_ARRAY[i], ONE_MSG_TIMEOUT) == -1)
			return -1;
}

/**
* @param TIMEOUT timeout in us
* @retval -1 if nothing got or timeout
*/
int8_t SPI_read(int32_t TIMEOUT)
{
	// *******************  SPSR – SPI Status Register *****************************
	// SPIF: SPI Interrupt Flag
	// WCOL: Write COLlision Flag // Is written, if SPDR is written while transfer
	
	// Wait for transfer start
	for (int i = 0; SPI_SS_STATE | !(SPSR & (1<<WCOL));i++)
		if (i <= TIMEOUT)
			_delay_us(1);
		else
			return -1;
	
	// Check if it's self addr and receive
	if (SPDR == SPI_SELF_ADDRESS)
		return SPDR;
	else
		return -1;
}



// ********************************************************************************************
// ******************************             I2C                ******************************
// ********************************************************************************************
// TODO:
// Сделать нормально
void I2C_init()
{
	TWBR = 0x62;//baud rate
	TWCR = 1<<TWEN;//enable i2c
	TWSR = 0x00;//seting prescaler to zero
}

void I2C_start()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA) | (1<<TWEA); //start condition
	while(!(TWCR &(1<<TWINT)));
}
void I2C_write(char Data)
{
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

int8_t I2C_read()
{
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

void I2C_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<TWSTO);
	while(!(TWCR & (1<<TWINT)));
}

#endif // AVR_FAST_CONFIG_H
