#ifndef THREADS_H
#define THREADS_H

/// USB interface exchange operations
#include "usb_operations.h"

/// Functions such as float to char converter
#include "utility_functions.h"

/// HAL library
#include "stm32f4xx_hal.h"

/// For FreeRTOS CMSIS_V2 functions
#include "cmsis_os.h"

extern TaskHandle_t MeasureThreadHandle; ///< Measurement task handler
extern TaskHandle_t MainThreadHandle; ///< Main task handler
extern TaskHandle_t ThermometerThreadHandle; ///< Thermometer measure thread

extern osThreadAttr_t MainThreadAttributes; ///< Main task configuration structure
extern osThreadAttr_t MeasureThreadAttributes; ///< Measurement task configuration structure
extern osThreadAttr_t ThermometerThreadAttributes; ///< Thermometer task configuration structure

// extern uint8_t buffer[64]; ///< USB messages buffer
extern ADC_HandleTypeDef * meas_hadc; ///< ADC for alcohol and pressure sensors
extern I2C_HandleTypeDef * temperature_hi2c; ///< MLX90614 sensor hi2c
extern TIM_HandleTypeDef * pomp_htim; ///< Pomp PWM generation timer handler

extern uint8_t buffer[64];

/**
 * @brief Main thread, works by exchange protocol with PC by USB bus
 */
void MainThread(void const * argument);

/**
 * @brief Measurement thread, measures alcohol and temperature
 */
void MeasureThread(void const * argument);

/**
 * @brief Thermometer thread, measures temperature by I2C from MLX90614
 */
void ThermometerThread(void const * argument);

/**
 * @brief Interpret JSON key got to command code
 *
 * @param command_text String with text to interpret
 *
 * @retval Command code
 */
int8_t translateReceivedCommand(char * command_text);

/**
 * @brief Reads temperature from MLX90614 sensor
 *
 * @param MLX90614_hi2c I2C handler with MLX90614 on I2C bus it points
 *
 * @retval Temperature, if errors, will return 310, 320 or 330 value
 *
 * @note Returns 310 if sensor is not on bus (I2C device ready error)
 * @note Returns 320 if sensor didn't got value (I2C device write to mem error)
 * @note Returns 330 if sensor didn't send value (I2C device read from mem error)
 */
float getTemperature(I2C_HandleTypeDef * MLX90614_hi2c);


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

#endif // THREADS_H
