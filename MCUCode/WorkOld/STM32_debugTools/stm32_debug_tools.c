#include "stm32_debug_tools.h"
#ifdef STM32_DEBUG_TOOLS_H
// Created on 15.02.2023


/// ******************** LED THINGS ******************** ///
#ifdef DEBUG_LED

void blinkRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); HAL_Delay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); HAL_Delay(BLINK_DELAY); }
void setRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); }
void resetRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); }

void blinkBlue() 	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); HAL_Delay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); HAL_Delay(BLINK_DELAY); }
void setBlue()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); }
void resetBlue()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); }

void blinkGreen()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); HAL_Delay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); HAL_Delay(BLINK_DELAY); }
void setGreen()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); }
void resetGreen()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); }

void blinkOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); HAL_Delay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); HAL_Delay(BLINK_DELAY); }
void setOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); }
void resetOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); }

#endif // DEBUG_LED











#ifdef DEBUG_I2C

int8_t I2C_DeviceScanner(I2C_HandleTypeDef *scan_hi2c)
{
	int8_t result = 0;

	for (uint16_t i = 0; i<256; i++)
	{
		result = HAL_I2C_IsDeviceReady(scan_hi2c, (uint16_t) (i<<1), 2, 2);

		if (result == HAL_OK)
		{
		  return i;
		}
	}

	return -1;
}

#endif // DEBUG_I2C














#ifdef DEBUG_USB_SEND

int8_t usb_get(uint8_t * usb_buffer)
{
	int temp = 0; // For CRC cut-off
	if (usb_buffer[1] != '\0')
	{
		// CRC cut-off

		for (int i = 0; usb_buffer[i]; i++)
			temp = i;
		usb_buffer[temp] = '\0';

		return 0;
	}
	else
		return 1;
}

// !!! DEPRECATED !!! //
// Used in receive function
int8_t usb_CRC_check(uint8_t * usb_buffer)
{
	int temp = 0;
	int8_t crc_calculated = 0x0;

	for (int i = 0; usb_buffer[i]; i++)
		temp = i;

	// CRC calculate algorythm
	for (int crc_table_pos = 0, i = 0; usb_buffer[i]; i++)
	{
		crc_table_pos = (crc_calculated ^ usb_buffer[i]) & 0xFF;
		crc_calculated = (crc_table[crc_table_pos] ^ (crc_calculated << 8)) & 0xFF;
	}

	return (crc_calculated & 0xFF != usb_buffer[temp]);
}


int8_t usb_bufferClear(uint8_t * buffer)
{
	for (int i = 0; buffer[i]; i++)
		buffer[i] = '\0';
}

 int8_t usb_send(char * msg)
{
	if (CDC_Transmit_FS(msg, strlen(msg)) == USBD_OK)
		return 0;
	else
		return 1;
}

#endif // DEBUG_USB_SEND











#ifdef DEBUG_ADC

// Remember ADC handler to work with it
 void setMainADC(ADC_HandleTypeDef * adc_handler, uint16_t * _adc_data, uint8_t _adc_channel_count)
{
	adc_main_handler = adc_handler;
	adc_data = _adc_data;
	adc_channel_count = _adc_channel_count;
}

// Returns ADC value
 float adc_getMeasure(uint16_t * adc_arr, uint8_t adc_channel)
{
	HAL_ADC_Start_IT(adc_main_handler);
	float inv_4096 = 1.0 / 4096.0; // Use ADC convertion time
	HAL_ADC_PollForConversion(adc_main_handler,10);

	return (float)adc_arr[adc_channel]*3.3 * inv_4096;
}


float adc_getMeasure_noHandler(ADC_HandleTypeDef * adc_handler, uint16_t * adc_arr, uint8_t adc_channel)
{
	HAL_ADC_Start_IT(adc_handler);
	float inv_4096 = 1.0 / 4096.0; // Use ADC convertion time
	HAL_ADC_PollForConversion(adc_handler,10);

	return (float)adc_arr[adc_channel]*3.3 * inv_4096;
}


// Callback function
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc)
{
	if (hadc = adc_main_handler)
	{
		adc_data[adc_current_channel] = hadc->Instance->DR;
		if (adc_current_channel < adc_channel_count)
			adc_current_channel++;
		else
			adc_current_channel = 0;
	}
}

#endif // DEBUG_ADC














// ******************** Other tools ***************** //

#ifdef DEBUG_SSD1306

void display_init(I2C_HandleTypeDef * display_hi2c)
{
	SSD1306_Init(display_hi2c);
}

void display_write(char * msg, int8_t posX, int8_t posY)
{
	SSD1306_GotoXY(posX, posY);
	SSD1306_Puts(msg, &Font_7x10, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(1);
}

void display_clear()
{
	SSD1306_Clear();
}

#endif // DEBUG_SSD1306




// ******************** Util functions ***************** //
#ifdef DEBUG_UTIL_FUNCTIONS

char * float_to_char(float what)
{
	char * result = (char*)malloc(6*sizeof(char));

	int what_toInt = what * 100.0;

	result[5] = '\0';

	result[4] = what_toInt % 10 + '0';
	what_toInt /= 10;

	result[3] = what_toInt % 10 + '0';
	what_toInt /= 10;

	result[2] = ',';

	result[1] = what_toInt % 10 + '0';
	what_toInt /= 10;

	result[0] = what_toInt % 10 + '0';

	return result;
}


int8_t checkStrEqual_fromTo(uint8_t * main_arr, uint8_t * internal_arr, int from, int to)
{
    int8_t result = 1;

    for (int check_pos = from; main_arr[check_pos] && internal_arr[check_pos - from] && result; check_pos++)
    {
        result = main_arr[check_pos] == internal_arr[check_pos - from];
    }
    return result;
}

#endif // DEBUG_UTIL_FUNCTIONS


#endif // STM32_DEBUG_TOOLS_H
