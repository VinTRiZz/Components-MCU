#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void i2c_init(int32_t baud_rate); // 0x62

int8_t i2c_start(uint16_t timeout);

int8_t i2c_write(char Data, uint16_t timeout);

char i2c_read();

int8_t i2c_stop(uint16_t timeout);

int8_t i2c_IsDeviceReady(uint8_t addr, uint16_t timeout); // timeout is in ms
#endif