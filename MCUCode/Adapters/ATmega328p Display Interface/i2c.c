#ifndef I2C_C
#define I2C_C

#include "i2c.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void i2c_init(int32_t baud_rate) // 0x62
{
	TWBR = baud_rate;//baud rate
	TWCR = 1<<TWEN;//enable i2c
	TWSR = 0x00;//seting prescaler to zero
}

int8_t i2c_start(uint16_t timeout)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA) | (1<<TWEA);//start condition
	for (uint16_t i = 0; i < timeout; i++)
	{
		if (!(TWCR &(1<<TWINT)))
		_delay_us(1);
		else
		{
			return 0;
		}
	}
	return 1;
}

int8_t i2c_write(char Data, uint16_t timeout)
{
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	for (uint16_t i = 0; i < timeout; i++)
	{
		if (!(TWCR &(1<<TWINT)))
		_delay_us(1);
		else
		{
			return 0;
		}
	}
	return 1;
}

char i2c_read()
{
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

int8_t i2c_stop(uint16_t timeout)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<TWSTO);
	for (uint16_t i = 0; i < timeout; i++)
	{
		if (!(TWCR &(1<<TWINT)))
		_delay_us(1);
		else
		{
			return 0;
		}
	}
	return 1;
}

int8_t i2c_IsDeviceReady(uint8_t addr, uint16_t timeout) // timeout is in ms
{
	if (i2c_write(addr, timeout))
	return 0;
	return 1;
}

#endif I2C_C