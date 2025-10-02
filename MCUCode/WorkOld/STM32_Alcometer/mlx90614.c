/*
 * mlx90614.c
 *
 *  Created on: Oct 5, 2021
 *      Author: falls
 */
#include "mlx90614.h"

char temp_buff[128] = {};

#ifdef TEST_NOT_SUCCESS
uint8_t CRC8_Calc (uint8_t *p, uint8_t len)
{
	uint16_t i;
	uint16_t crc = 0x0;

	while (len--)
	{
			i = (crc ^ *p++) & 0xFF;
			crc = (crc_table[i] ^ (crc << 8)) & 0xFF;
	}

	return crc & 0xFF;
}
#endif // TEST_NOT_SUCCESS

void MLX90614_WriteReg(uint8_t devAddr, uint8_t regAddr, uint16_t data, I2C_HandleTypeDef hi2c)
{
	uint8_t i2cdata[4], temp[4];

	temp[0] = (devAddr << 1);
	temp[1] = regAddr;

	temp[2] = 0x00;
	temp[3] = 0x00;

	// For a write word command, in the crc8 calculus, you have to include [SA_W, Command, LSB, MSB]

	i2cdata[0] = temp[1]; //EEPROM-address
	i2cdata[1] = temp[2]; //Delete-Byte, low
	i2cdata[2] = temp[3]; //Delete-Byte, high

#ifdef TEST_NOT_SUCCESS
	i2cdata[3] = CRC8_Calc(temp, 4);
#else
	i2cdata[3] = crc8(temp, 4);
#endif // TEST_NOT_SUCCESS

	HAL_I2C_Master_Transmit(&hi2c, (devAddr << 1), i2cdata, 4, 0xFFFF);
	HAL_Delay(10);

	temp[2] = data & 0xFF; //Getting LSB first
	temp[3] = data >> 8;   //Getting MSB after

	i2cdata[0] = temp[1]; //EEPROM-address
	i2cdata[1] = temp[2]; //Delete-Byte, low
	i2cdata[2] = temp[3]; //Delete-Byte, high

#ifdef TEST_NOT_SUCCESS
	i2cdata[3] = CRC8_Calc(temp, 4);
#else
	i2cdata[3] = crc8(temp, 4);
#endif // TEST_NOT_SUCCESS

	HAL_I2C_Master_Transmit(&hi2c, (devAddr << 1), i2cdata, 4, 0xFFFF);
	HAL_Delay(10);
}

uint16_t MLX90614_ReadReg(uint8_t devAddr, uint8_t regAddr, I2C_HandleTypeDef hi2c)
{
	uint16_t data;
	uint8_t in_buff[3], crc_buff[5], crc;

	HAL_I2C_Mem_Read(&hi2c, (devAddr<<1), regAddr, 1, in_buff, 3, 100);

	// For a read word command, in the crc8 calculus, you have to include [SA_W, Command, SA_R, LSB, MSB]
	crc_buff[0] = (devAddr<<1);
	crc_buff[1] = regAddr;
	crc_buff[2] = (devAddr<<1) + 1;
	crc_buff[3] = in_buff[0];
	crc_buff[4] = in_buff[1];

#ifdef TEST_NOT_SUCCESS
	crc = CRC8_Calc(crc_buff, 5);
#else
	crc = crc8(crc_buff, 5);
#endif // TEST_NOT_SUCCESS

	data = (in_buff[1] <<8 | in_buff[0]);

	//TODO: implement CRC8 check on data received
	if (crc != in_buff[2]) {
		data = 0x0000;
	}
	HAL_Delay(1);
	return data;
}

float MLX90614_ReadTemp(uint8_t devAddr, uint8_t regAddr, I2C_HandleTypeDef hi2c)
{
	float temp;
	uint16_t data;

	uint8_t tx_data[1];
	tx_data[0] = MLX90614_TOBJ1;
	uint8_t rx_data[3];

	HAL_I2C_Mem_Write(&hi2c, devAddr << 1, tx_data[0], I2C_MEMADD_SIZE_8BIT, tx_data, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c, devAddr << 1, tx_data[0], I2C_MEMADD_SIZE_8BIT, rx_data, 3, HAL_MAX_DELAY);

	data = MLX90614_ReadReg(devAddr, regAddr, hi2c);
	// data = (rx_data[1] << 8) | rx_data[0];

	temp = data*0.02 - 273.15; // from datasheet

	return temp;
}

double MLX90614_temp_compensation(double x)
{
	double terms[] = {
		4.5285558056644653e+001,
		-7.7381865153331808e-001,
		1.4986086918853544e-002
	};

	size_t csz = sizeof terms / sizeof *terms;

	double t = 1;
	double r = 0;
	for (int i = 0; i < csz;i++)
	{
		r += terms[i] * t;
		t *= x;
	}
	return r;
}
