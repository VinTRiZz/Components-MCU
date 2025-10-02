/// I2C Lib


#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

void i2c_init()
{
	TWBR = 0x62;//baud rate
	TWCR = 1<<TWEN;//enable i2c
	TWSR = 0x00;//seting prescaler to zero	
}

void i2c_start()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA) | (1<<TWEA);//start condition
	while(!(TWCR &(1<<TWINT)));	
}
void i2c_write(char Data)
{
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

char i2c_read()
{
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

void i2c_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<TWSTO);
	while(!(TWCR & (1<<TWINT)));
}
#endif










/// Main func


int main(void)
{
	int8_t clock_stretch = 0;
	int8_t MS_Byte = 0;
	int8_t LS_Byte = 0;
	
	DDRD = 0xFF; // test
	
	// init
	i2c_init();
	
	// main cycle
	while (1)
	{
		PORTD = com_to_start();
	}
	return 0;
 }



char com_to_start()
{
	char resH = 0;
	char resL = 0;
	
	i2c_start();
	i2c_write(TEST_ADRESS << 1);
	
	i2c_write(0xF3); // measure com
	
	i2c_start();
	i2c_write((TEST_ADRESS << 1) + 1);
	
	_delay_ms(10);
	
	resH = i2c_read();
	resL = i2c_read();
	
	i2c_stop();
	
	return (175.72*(512*resL + resH)) / 65536;
}
