#ifndef STM32_DEBUG_TOOLS_H
#define STM32_DEBUG_TOOLS_H

// Library made in need of abstraction

// COMPATIBLE ONLY WITH STM32F407G-Discovery


#include "stm32f4xx_hal.h"


/// ************************* SWITCHES ************************** ///

// For blink, set pin and etc at the STM32F407G-Discovery
// #define DEBUG_LED
// #define BLINK_DELAY 100

// To check devices connected
// #define DEBUG_I2C

// Te send message or receive it
// #define DEBUG_USB_SEND

// Work with 1 channel only
// #define DEBUG_ADC
// #define DEBUG_ADC_CALLBACK


// ********** Other utils ***** //
#define DEBUG_SSD1306

#define DEBUG_UTIL_FUNCTIONS

/// *********************** MODULE DEPENDS ********************** ///

#ifdef DEBUG_USB_SEND
#include "usb_device.h"
#endif // DEBUG_USB_SEND

#ifdef DEBUG_SSD1306
#include "ssd1306.h"
#endif // DEBUG_SSD1306





/// ******************** FUNCTION PROTOTYPES ******************** ///

/// ******************** Led things
#ifdef DEBUG_LED
void blinkRed();
void setRed();
void resetRed();

void blinkBlue();
void setBlue();
void resetBlue();

void blinkGreen();
void setGreen();
void resetGreen();

void blinkOrange();
void setOrange();
void resetOrange();
#endif // DEBUG_LED



/// ******************** I2C things
#ifdef DEBUG_I2C
int8_t I2C_DeviceScanner(I2C_HandleTypeDef scan_hi2c);
#endif // DEBUG_I2C




/// ******************** USB things
#ifdef DEBUG_USB_SEND

extern int8_t usb_get(uint8_t * usb_buffer);
extern int8_t usb_send(char * msg);
int8_t usb_bufferClear(uint8_t * buffer);

#endif // DEBUG_USB_SEND



#ifdef DEBUG_ADC

ADC_HandleTypeDef * adc_main_handler;

uint16_t * adc_data;

uint8_t adc_current_channel = 0; uint8_t adc_channel_count = 0;

// Setup ADC handler to work with it
void setMainADC(ADC_HandleTypeDef * adc_handler, uint16_t * _adc_data, uint8_t _adc_channel_count);

// Get measure with or without main adc hanlder set
float adc_getMeasure(uint16_t * adc_arr, uint8_t adc_channel);
float adc_getMeasure_noHandler(ADC_HandleTypeDef * adc_handler, uint16_t * adc_arr, uint8_t adc_channel);

#endif // DEBUG_ADC



/// ******************** Other tools
#ifdef DEBUG_SSD1306

void display_init(I2C_HandleTypeDef * display_hi2c);
void display_write(char * msg, int8_t posX, int8_t posY);
void display_clear();

#endif // DEBUG_SSD1306



/// ******************** Util functions, such as convertor, string comparisor, etc
#ifdef DEBUG_UTIL_FUNCTIONS

// Convertor, need to free memory from pointer contain return value (after use)
char * float_to_char(float what);

// Checks if equal
int8_t checkStrEqual_fromTo(uint8_t * main_arr, uint8_t * internal_arr, int from, int to);

#endif // DEBUG_UTIL_FUNCTIONS











#endif // STM32_DEBUG_TOOLS_H
