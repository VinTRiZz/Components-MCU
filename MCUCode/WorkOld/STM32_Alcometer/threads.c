#include "threads.h"

/// DEBUG

#define BLINK_DELAY 150

void blinkRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); osDelay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); osDelay(BLINK_DELAY); }
void setRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); }
void resetRed()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); }

void blinkBlue() 	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); osDelay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); osDelay(BLINK_DELAY); }
void setBlue()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); }
void resetBlue()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); }

void blinkGreen()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); osDelay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); osDelay(BLINK_DELAY); }
void setGreen()		{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); }
void resetGreen()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); }

void blinkOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); osDelay(BLINK_DELAY); HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); osDelay(BLINK_DELAY); }
void setOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); }
void resetOrange()	{ HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); }

/**
 * @brief Defines what pin to read with connected USB to it, for detecting connection
 */
#define USB_PIN_READ HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)

/**
 * @brief Measurement timeout in ms
 */
#define MEASURE_TIMEOUT 1000

/*
 * @brief Command codes
 */
#define COMMAND_RESET 1
#define COMMAND_INTERRUPT 2
#define COMMAND_PING 3
#define COMMAND_MEASURE 4
#define COMMAND_SEND 5

/*
 * @brief Error codes
 */
#define ALCOHOL_RESERVED_1 1
#define ALCOHOL_RESERVED_2 2
#define ALCOHOL_RESERVED_3 3


//TODO:  DEBUG SWITCH
#define THERMOMETER_ONLY



TaskHandle_t MeasureThreadHandle; ///< Measurement task handler
TaskHandle_t MainThreadHandle; ///< Main task handler
TaskHandle_t ThermometerThreadHandle; ///< Thermometer measure thread

osThreadAttr_t MainThreadAttributes =
{
  .name = "A_mainThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/**
 * @brief Measurement task configuration structure
 */
osThreadAttr_t MeasureThreadAttributes =
{
  .name = "B_measThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/**
 * @brief Measurement task configuration structure
 */
osThreadAttr_t ThermometerThreadAttributes = {
  .name = "C_thermoThread",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal, ///< High priority in case of I2C exchange
};

float temperature = 0; ///< Temperature measured

float alcohol = 0; ///< Alcohol measured

uint8_t measuring_flag = 0; ///< 1 if measure is running

uint8_t error_alcohol_code = 0; ///< Error code of alcohol sensor
uint8_t alcohol_interrupt_flag = 0; ///< Interrupt flag for alcohol thread


/// ************* Alcohol & pressure sensor ***************** ///


#define ALCOHOL_CHANNEL_NUMBER 0
#define PRESSURE_CHANNEL_NUMBER 1

// Value minimum on pressure sensor, 80 mm = 10665.8 Pa;
// 5 mA on sensor is ~80 kPa; 5 mA is ~6 ADC
// TODO: Need amplifier, too small number
#define ADC_MINIMAL_PRESSURE 6

// Idle value on alcohol sensor, ~0.18 V
#define ADC_MINIMAL_ALCOHOL 250

#define ADC_CHANNEL_COUNT 2

int16_t adc[ADC_CHANNEL_COUNT] = {0};

int8_t adc_ch = 0;


// Callback for ADC reading
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == meas_hadc)
	{
		adc[adc_ch] = hadc->Instance->DR;

		if (adc_ch + 1 < ADC_CHANNEL_COUNT)
			adc_ch++;
		else
			adc_ch = 0;
	}
}


/// ********************** Thermometer *********************** ///

#define MLX_I2C_ADDRESS 0x5A

/**
 * @brief Defines how much time must wait before stop measiring
 */
#define THERMOMETER_MEASURE_TIMEOUT 500

float getTemperature(I2C_HandleTypeDef * MLX90614_hi2c)
{
	float result = 0;
	HAL_StatusTypeDef state = HAL_OK;

	state = HAL_I2C_IsDeviceReady(MLX90614_hi2c, MLX_I2C_ADDRESS << 1, 100, 1000);
	if (state == HAL_OK)
	{
		uint8_t databuf[3] = {0}; ///< Data buffer for read/write to MLX

		// Send command to start object temperature measure
		state = HAL_I2C_Mem_Write(MLX90614_hi2c, MLX_I2C_ADDRESS << 1, 1, 1, NULL, 0, 1000);
		if (state == HAL_OK)
		{
			databuf[0] = 0;

			// Wait for ready
			for (int i = 0; (i < THERMOMETER_MEASURE_TIMEOUT) && !(databuf[0] & 0x40); i++)
			{
				state = HAL_I2C_Mem_Read(MLX90614_hi2c, MLX_I2C_ADDRESS << 1, 0x06, 1, databuf, 2, 1000);
				if (state != HAL_OK)
				{
					break; // Means error
				}
			}

			// Read temperature data (little-endian format)
			databuf[0] = 0;
			state = HAL_I2C_Mem_Read(MLX90614_hi2c, MLX_I2C_ADDRESS << 1, 0x07, 1, databuf, 2, 1000);
			if (state == HAL_OK)
			{
				uint16_t raw_temp = databuf[0] | (databuf[1] << 8);

				result = (float)raw_temp * 0.02 - 273.15;
			}
			else
			{
				result = -330; // Really error value with error code 30 (read error)
			}
		}
		else
		{
			result = -320; // Really error value with error code 20 (write error)
		}
	}
	else
	{
		result = -310; // Really error value with error code 10 (ready error)
	}

	return result;
}


/// ********************** Main thread *********************** ///
void MainThread(void const * argument)
{
	int16_t DELAY_PERIOD = 1; ///< Period of asking if there's a message (for sleep mode)

	char messageBuf[64] = {'\0'}; ///< Message buffer

	char response_string[20] = {'\0'}; ///< For sending

	char request_string[20] = {'\0'}; ///< For receiving

	int8_t command_code = 0;

	// Wait for usb
	while(USB_PIN_READ != GPIO_PIN_SET );

	while (1)
	{
		setBlue();
		// Wait for messages from PC
		while (!usb_read_no_crc(messageBuf))
		{
			if ( (DELAY_PERIOD >= 0) && (DELAY_PERIOD <= 100000) )
				osDelay(DELAY_PERIOD);
			else
				osDelay(0);
		}
		resetBlue();

		// Parse command to it's code and handle it
		if (parse_json_key("command", request_string, messageBuf) > 0)
		{
			command_code = translateReceivedCommand(request_string);

			// ************* Interpret command code ************** //
			if (command_code == COMMAND_RESET)
			{
				send_json("Reset_requested");
				NVIC_SystemReset();

			} else if (command_code == COMMAND_INTERRUPT)
			{
				send_json("Interrupt_requested");

				alcohol_interrupt_flag = 1;

			} else if (command_code == COMMAND_PING)
			{
				send_json("Ping_requested");

			} else if (command_code == COMMAND_MEASURE)
			{
				send_json("Measure_requested");
				vTaskResume( MeasureThreadHandle ); // Starts measurement

			} else if (command_code == COMMAND_SEND)
			{
				// Wait for thread to end up measurement or timeout
				for (int i = 0; (i < MEASURE_TIMEOUT) && measuring_flag; i++)
				{
					osDelay(1);
				}

				send_json("Send_requested");
				// Check if MLX has errors
				if (temperature > 0)
				{
					send_json("success[MLX]");
					float_to_char(temperature, response_string, 2);
					send_json(response_string);

				} else
				{
					send_json("error[MLX_I2C_ERROR]");
				}

				// Check if alcohol sensor has errors
				if (error_alcohol_code == 0)
				{
					send_json("success[ALCOHOL]");
					float_to_char(alcohol, response_string, 3);
					send_json(response_string);

				} else if (error_alcohol_code == ALCOHOL_RESERVED_1)
				{
					send_json("error[ERROR_RESERVED_1]");
				} else if (error_alcohol_code == ALCOHOL_RESERVED_2)
				{
					send_json("error[ERROR_RESERVED_2]");
				} else if (error_alcohol_code == ALCOHOL_RESERVED_3)
				{
					send_json("error[ERROR_RESERVED_3]");
				} else
				{
					send_json("error[ALCOHOL_UNKNOWN_ERROR]");
				}
			} else
			{
				send_json("error[INTERPRET_ERROR]");
			}
		}
		else
		{
			send_json("error[INVALID_DATA]");
		}

		usb_clearBuffer();
	}
}

/**
 * @brief Defines count of measures controller will do
 */
#define ALCOHOL_MEASURE_COUNT 15

/**
 * @brief Defines period in ms of alcohol measuring
 */
#define ALCOHOL_MEASURE_PERIOD 10

/**
 * @brief Defines time pomp need to get air
 */
#define POMP_AIR_CAPTURE_TIME_MS 10


void MeasureThread(void const * argument)
{
	// ADC initialization
	if (meas_hadc == NULL)
	{
		while (1)
		{
			vTaskSuspend( NULL ); // Error caused by bad init
		}
	}

#ifndef THERMOMETER_ONLY
	float alcohol_measuresArr[ALCOHOL_MEASURE_COUNT] = {0}; ///< Measure values

	float inv_4096 = 1.0 / 4096.0; ///< For count time saving

	uint32_t current_time = 0; ///< For time catching with pomp (using function HAL_GetTick() )
#endif // THERMOMETER_ONLY

	while (1)
	{
		measuring_flag = 0; // Signal that measure ended or not started
		vTaskSuspend( NULL ); // Wait for measure start signal
		setOrange(); // TODO: DEBUG
		measuring_flag = 1; // Signal that measure started

#ifndef THERMOMETER_ONLY
		// Wait for pressure sensor
		while ((adc[PRESSURE_CHANNEL_NUMBER] < ADC_MINIMAL_PRESSURE) && (!alcohol_interrupt_flag))
			osDelay(1);

		// Start generating PWM for pomp
		HAL_TIM_PWM_Start(pomp_htim, TIM_CHANNEL_1);

		// Remember time pomp started to capture air
		current_time = HAL_GetTick();
#endif // THERMOMETER_ONLY

		// Get temperature
		vTaskResume( ThermometerThreadHandle ); // Start temperature measurement

#ifndef THERMOMETER_ONLY
		// Wait for pomp
		while ( current_time - HAL_GetTick() > POMP_AIR_CAPTURE_TIME_MS);

		// Stop generating PWM for pomp
		HAL_TIM_PWM_Stop(pomp_htim, TIM_CHANNEL_1);

		// Get alcohol data
		for (int i = 0; (i < ALCOHOL_MEASURE_COUNT) && (!alcohol_interrupt_flag); i++)
		{
			alcohol_measuresArr[i] = (adc[ ALCOHOL_CHANNEL_NUMBER ] * 3.0f) * inv_4096 ;

			osDelay(ALCOHOL_MEASURE_PERIOD); // Wait before next measure
		}

		// Calculate alcohol
		for (int i = 0; (i < ALCOHOL_MEASURE_COUNT) && (!alcohol_interrupt_flag); i++)
		{
			/* INFO FROM NEURAL NET:
			 C = (Rs/R0 - 1)/α
			 где
				C - концентрация паров алкоголя в воздухе (в мг/л),
				Rs - сопротивление датчика при измерении,
				R0 - номинальное сопротивление датчика при отсутствии алкоголя (обычно 10 кОм),
				α - коэффициент чувствительности датчика к алкоголю (обычно 0.5).
			 */
			alcohol_measuresArr[i] = alcohol_measuresArr[i]; // TODO: Edit in case of measurement info
		}

		alcohol_interrupt_flag = 0;
#endif // THERMOMETER_ONLY

		resetOrange(); // TODO: DEBUG
	}
}


void ThermometerThread(void const * argument)
{
	temperature = -1;

	if (temperature_hi2c == NULL)
	{
		while (1)
		{
			vTaskSuspend( NULL ); // Error caused by bad initialisation
		}
	}

	while (1)
	{
		vTaskSuspend( NULL ); // Wait for measure start signal

		temperature = getTemperature(temperature_hi2c);
	}
}



int8_t translateReceivedCommand(char * command_text)
{
	// Read command code in priorities of command text can get
	if ( checkStrEqual_fromTo(command_text, "reset", 0, strlen("reset") ) )
	{
		return COMMAND_RESET;
	} else if ( checkStrEqual_fromTo(command_text, "interrupt", 0, strlen("interrupt") ) )
		{
			return COMMAND_INTERRUPT;
		} else if ( checkStrEqual_fromTo(command_text, "ping", 0, strlen("ping") ) )
			{
				return COMMAND_PING;
			} else if ( checkStrEqual_fromTo(command_text, "measure", 0, strlen("measure") ) )
				{
					return COMMAND_MEASURE;
				} else if ( checkStrEqual_fromTo(command_text, "send", 0, strlen("send") ) )
					{
						return COMMAND_SEND;
					}
	return 0; // Didn't recognized command
}
