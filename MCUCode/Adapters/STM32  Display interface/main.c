/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// Display headers
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

// Screen characteristics
#define SCREEN_WIDTH 128
#define SCREEN_HIGH 64

// Positions of displayed data
#define BATTERY_POS_Y 0
#define BATTERY_POS_X 100
// Y
#define PWM1_POS 10
#define PWM2_POS 20
#define T1_POS 30
#define T2_POS 40

// Pixels from right screen limit for cleaning data fields
#define PWM_DATA_CLEAR 49
#define TEMPERATURE_DATA_CLEAR 63

// Battery voltage limits
#define BATTERY_MIN 2.1f // Minimal is 2.0
#define BATTERY_LOW 2.3f
#define BATTERY_MED 2.9f
#define BATTERY_HIGH 3.3f
#define BATTERY_MAX 3.6f // Maximum is 3.7

// Equ of 1 v for ADC
#define ADC_V_EQU 1.0f

// Maximum pulse, that can be in PWM generators
#define MAXIMUM_PULSE 127

// Temperature limits of temp resistors
#define T_MAX 100
#define T_MIN 30

// Resistor in a voltage divider, Om
#define Rb 20000.0f
// Voltage in a voltage divider input, V
#define Ub 5.0f


// For changing PWM pulse
TIM_OC_InitTypeDef sConfigOC;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

int16_t getTemperature(int8_t T);

// Display (sending data):
void SSD1306_BAT_SHOW(float ADC_DATA);
void SSD1306_PWM_SEND(int8_t PWM, int8_t channel);
void SSD1306_T_SHOW(int16_t temperature1, int16_t temperature2);

// Clears data shown by prev functions
void SSD1306_ClearData(int8_t PLACE);

// PWM (changing pulse of a pair):
// freq = 16*10^6 / (1 * 128) = 125*10^3 Hz
// Pulse == between 0 and 127
HAL_StatusTypeDef changePulse_ch1(int8_t value);
HAL_StatusTypeDef changePulse_ch2(int8_t value);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int16_t adc[3] = {0};
int8_t adc_ch = 0;
int16_t res = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	switch (adc_ch)
	{
	case 0:
		adc[0] = hadc1.Instance->DR;
		break;

	case 1:
		adc[1] = hadc1.Instance->DR;
		break;

	case 2:
		adc[2] = hadc1.Instance->DR;
		break;

	default:
		adc_ch = 0;
		adc[0] = hadc1.Instance->DR;
	}
	adc_ch++;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  // ----------------Buttons:
  /*
   * Init 'em up
   */

  // ----------------Display:
  SSD1306_Init();
  HAL_Delay(5);

  // ----------------PWM:
  int8_t PWM1_ON = 0;
  int8_t PWM2_ON = 0;

  // PWM pulse buffers
  int8_t pwm_ch1 = 0;
  int8_t pwm_ch2 = 0;

  // -----------------ADC:
  // Result buffer for channels
  float adc_battery = 0;
  uint16_t adc_temp1 = 0;
  uint16_t adc_temp2 = 0;
  // Saves values to change display when they change
  uint16_t buf_adc_temp1 = 0;
  uint16_t buf_adc_temp2 = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // Show static text

  SSD1306_GotoXY(3,PWM1_POS);
  SSD1306_Puts("Яркость 1: ", &Font_7x10,1);
  SSD1306_Puts("Выкл.",&Font_7x10,1);

  SSD1306_GotoXY(3,PWM2_POS);
  SSD1306_Puts("Яркость 2: ", &Font_7x10,1);
  SSD1306_Puts("Выкл.",&Font_7x10,1);


  SSD1306_GotoXY(3,T1_POS);
  SSD1306_Puts("Темп. 1: ", &Font_7x10,1);
  SSD1306_GotoXY(3,T2_POS);
  SSD1306_Puts("Темп. 2: ", &Font_7x10,1);
  SSD1306_T_SHOW(adc_temp1,adc_temp2);



  /*
   * Pins:
   * E 14 -- PWM1 on / off
   * E 15 -- PWM2 on / off
   * E 12 -- PWM1 down
   * E 13 -- PWM2 down
   * E 8 -- PWM1 up
   * E 10 -- PWM2 up
  */

  int8_t str_temperature[8];

  while (1)
  {
		// Activate or deactivate PWM1
		if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) != GPIO_PIN_RESET)
		{
	  		PWM1_ON = !PWM1_ON;
	  		if (PWM1_ON)
	  		{
	  			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  			SSD1306_ClearData(1);
	  			SSD1306_PWM_SEND(pwm_ch1,0);
	  		}
	  		else
	  		{
	  			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	  			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
				SSD1306_Puts("Выкл.",&Font_7x10,1);
	  		}

	  		while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) != GPIO_PIN_RESET)
	  			HAL_Delay(100);
		}

		// Activate or deactivate PWM2
	  	if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) != GPIO_PIN_RESET)
	  	{
			PWM2_ON = !PWM2_ON;
			if (PWM2_ON)
			{
				HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
				SSD1306_ClearData(2);
				SSD1306_PWM_SEND(pwm_ch2,1);
			}
			else
			{
				HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
				SSD1306_Puts("Выкл.",&Font_7x10,1);
			}


			while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) != GPIO_PIN_RESET)
				HAL_Delay(100);
	  	}

	  	if (PWM1_ON)
	  	{
			// Change pulse
			if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_8) != GPIO_PIN_RESET)
			{
			  if (pwm_ch1 < 100)
				  pwm_ch1++;
			  else
				  pwm_ch1 = 100;
			  changePulse_ch1(pwm_ch1);
			  SSD1306_ClearData(1);
			  SSD1306_PWM_SEND(pwm_ch1,0);
			  HAL_Delay(1000);
			  while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_8) != GPIO_PIN_RESET) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch1 < 99)
					  pwm_ch1+=2;
				  else
					  pwm_ch1 = 100;
				  changePulse_ch1(pwm_ch1);
				  SSD1306_ClearData(1);
				  SSD1306_PWM_SEND(pwm_ch1,0);
				  HAL_Delay(20);
			  }
			}

			if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) != GPIO_PIN_RESET)
			{
			  if (pwm_ch1 > 0)
				  pwm_ch1--;
			  else
				  pwm_ch1 = 0;
			  changePulse_ch1(pwm_ch1);
			  SSD1306_ClearData(1);
			  SSD1306_PWM_SEND(pwm_ch1,0);
			  HAL_Delay(1000);
			  while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) != GPIO_PIN_RESET) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch1 > 1)
					  pwm_ch1-=2;
				  else
					  pwm_ch1 = 0;
				  changePulse_ch1(pwm_ch1);
				  SSD1306_ClearData(1);
				  SSD1306_PWM_SEND(pwm_ch1,0);
				  HAL_Delay(20);
			  }
			}
	  	}

	  	if (PWM2_ON)
	  	{
			if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_10) != GPIO_PIN_RESET)
			{
			  if (pwm_ch2 < 100)
				  pwm_ch2++;
			  else
				  pwm_ch2 = 100;
			  changePulse_ch2(pwm_ch2);
			  SSD1306_ClearData(2);
			  SSD1306_PWM_SEND(pwm_ch2,1);
			  HAL_Delay(1000);
			  while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_10) != GPIO_PIN_RESET) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch2 < 99)
					  pwm_ch2+=2;
				  else
					  pwm_ch2 = 100;
				  changePulse_ch2(pwm_ch2);
				  SSD1306_ClearData(2);
				  SSD1306_PWM_SEND(pwm_ch2,1);
				  HAL_Delay(20);
			  }
			}

			if (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_13) != GPIO_PIN_RESET)
			{
			  if (pwm_ch2 > 0)
				  pwm_ch2--;
			  else
				  pwm_ch2 = 0;
			  changePulse_ch2(pwm_ch2);
			  SSD1306_ClearData(2);
			  SSD1306_PWM_SEND(pwm_ch2,1);
			  HAL_Delay(1000);
			  while (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_13) != GPIO_PIN_RESET) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch2 > 1)
					  pwm_ch2-=2;
				  else
					  pwm_ch2 = 0;
				  changePulse_ch2(pwm_ch2);
				  SSD1306_ClearData(2);
				  SSD1306_PWM_SEND(pwm_ch2,1);
				  HAL_Delay(20);
			  }
			}
	  	}

		// Get data from ADC
		// INPUT VOLTAGE = (ADC Value / ADC Resolution) * Reference Voltage
	  	// For ask if prev value was diff to new
	  	buf_adc_temp1 = adc_temp1;
	  	buf_adc_temp2 = adc_temp2;
	  	// Get new values
	  	HAL_ADC_Start_IT(&hadc1);
	  	HAL_ADC_PollForConversion(&hadc1,10);
			adc_battery = ((float)adc[0]*3) / 4096;

		HAL_ADC_Start_IT(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,10);
			adc_temp1 = getTemperature(0);

		HAL_ADC_Start_IT(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,10);
			adc_temp2 = getTemperature(1);

		HAL_Delay(1); // For display

		// Show battery on display
		SSD1306_BAT_SHOW(adc_battery);

		// Show temperature
		if (buf_adc_temp1 != adc_temp1)
		  {
			  SSD1306_ClearData(3);
			  SSD1306_T_SHOW(adc_temp1,adc_temp2);
		  }
		if (buf_adc_temp2 != adc_temp2)
		{
			SSD1306_ClearData(4);
			SSD1306_T_SHOW(adc_temp1,adc_temp2);
		}
		SSD1306_UpdateScreen();
		HAL_Delay(50);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 159;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 127;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 63;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE8 PE10 PE12 PE13
                           PE14 PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief Clears numbers shown in a place
 * @param PLACE Means place, where clean data
 * @note PLACE can be:
 * 						1 -- PWM 1
 * 						2 -- PWM 2
 * 						3 -- TEMPERATURE 1
 * 						4 -- TEMPERATURE 2
 */
void SSD1306_ClearData(int8_t PLACE)
{
	switch (PLACE)
	{
	case 1: // PWM 1
		SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
		for (int i = PWM_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
			{
				SSD1306_Putc(' ',&Font_7x10,1);
				HAL_Delay(1);
			}
		break;

	case 2: // PWM 2
		SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
		for (int i = PWM_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
			{
				SSD1306_Putc(' ',&Font_7x10,1);
				HAL_Delay(1);
			}
		SSD1306_UpdateScreen();
		break;

	case 3: // TEMPERATURE 1
		SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T1_POS);
		for (int i = TEMPERATURE_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
			{
				SSD1306_Putc(' ',&Font_7x10,1);
				HAL_Delay(1);
			}
		SSD1306_UpdateScreen();
		break;

	case 4: // TEMPERATURE 2
		SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T2_POS);
		for (int i = TEMPERATURE_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
				{
					SSD1306_Putc(' ',&Font_7x10,1);
					HAL_Delay(1);
				}
		SSD1306_UpdateScreen();
		break;
	}
}



/**
 * @brief Sends data from ADC pin to SSD_1306 by I2C
 * @param ADC_DATA Data to send
 * @note Depends on @ref BATTERY_U
 */
void SSD1306_BAT_SHOW(float ADC_DATA)
{
	SSD1306_GotoXY(BATTERY_POS_X,BATTERY_POS_Y);

	if (ADC_DATA > BATTERY_MIN)
	{
		if (ADC_DATA > BATTERY_LOW)
		{
			if (ADC_DATA > BATTERY_MED)
			{
				if (ADC_DATA > BATTERY_HIGH)
				{
					if (ADC_DATA > BATTERY_MAX)
						SSD1306_Puts("Overdrive",&Font_7x10,1); // Too high
					else
						SSD1306_Putc(0,&Pictures_7x10,1); // Full bat
				}
				else
					SSD1306_Putc(1,&Pictures_7x10,1); // Pre-full bat
			}
			else
				SSD1306_Putc(2,&Pictures_7x10,1); // Med bat
		}
		else
			SSD1306_Putc(3,&Pictures_7x10,1); // Pre-min bat
	}
	else
		SSD1306_Putc(4,&Pictures_7x10,1); // Min bat

	SSD1306_UpdateScreen();
	HAL_Delay(1); // For display
}


/**
 * @brief Sends PWM data to SSD_1306 by I2C
 * @param PWM Data from first PWM to send, %
 * @param channel Channel to send data: 0 -- First
 * 										1 -- Second
 * @note Presenting template:
 * 		 	VT_1 = 000
 * 			VT_2 = 000
 */
void SSD1306_PWM_SEND(int8_t PWM, int8_t channel)
{
	if (PWM > 0)
	{
		if (PWM >= 100)
		{
			if (!channel)
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
			else
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
			SSD1306_Puts("100%", &Font_7x10, 1);
			HAL_Delay(1); // For display
		}
		else
		{
			int8_t str_PWM[2];
			str_PWM[1] = PWM % 10 + '0';
			PWM /= 10;
			str_PWM[0] = PWM % 10 + '0';
			str_PWM[2] = '\0';

			if (!channel)
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
			else
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
			SSD1306_Puts(str_PWM, &Font_7x10, 1);
			HAL_Delay(1); // For display
			SSD1306_Putc ('%', &Font_7x10, 1);
			HAL_Delay(1); // For display
		}
	}
	else
		if (!channel)
			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
		else
		{
			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
			SSD1306_Puts("0%", &Font_7x10, 1);
			HAL_Delay(1); // For display
		}

    SSD1306_UpdateScreen();
    HAL_Delay(1); // For display
}


HAL_StatusTypeDef changePulse_ch1(int8_t value) // For ch 1 tim 1
{
	if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK)
    	return HAL_ERROR;
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;
	// now 0 <= value <= 100

	float buf = ((float)value / 100) * MAXIMUM_PULSE;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    if (value > 0)
    	sConfigOC.Pulse = (int16_t)buf;
    else
    	sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    	return HAL_ERROR;
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
	    	return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef changePulse_ch2(int8_t value) // For ch 1 tim 1
{
	if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2) != HAL_OK)
    	return HAL_ERROR;
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;
	// now 0 <= value <= 100

	float buf = ((float)value / 100) * MAXIMUM_PULSE;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    if (value > 0)
    	sConfigOC.Pulse = (int16_t)buf;
    else
    	sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    	return HAL_ERROR;
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
	    	return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief
 * @param T Temperature to show, 0 for first and 1 to second
 * @retval Temperature value. Template: T=31.7; retval=317
 */
int16_t getTemperature(int8_t T)
{
	// T( R ) == -22.54 * ln( R ) + 273.81 // Approximated curve
	float buf = 0;
	if (T)
	{
		buf = (adc[1] * 3.0f) / 4096.0f;
		buf = (5.0f - buf) * Rb / (buf * 1.0f);
		buf = -22.54f * log(buf*1.0f) + 273.81f;
	}
	else
	{
		buf = (adc[2] * 3.0f) / 4096.0f;
		buf = (5.0f - buf) * Rb / (buf * 1.0f);
		buf = -22.54f * log(buf*1.0f) + 273.81f;
	}
	if (buf <= T_MIN)
		return T_MIN*100;
	if (buf >= T_MAX)
		return T_MAX*100;
	return (int16_t)(buf * 100);
}

/**
 * @brief Sends temperature value
 * @param temperature Temperature to show
 * @note Temperature can be from from 30 to 100, otherwise
 * 		 it will show 30 or 100
 */
void SSD1306_T_SHOW(int16_t temperature1, int16_t temperature2)
{
	int8_t temp_buf = temperature1 / 10;
	int8_t str_temperature[8];
	str_temperature[4] = temperature1 % 10 + '0'; // ,00
	temperature1 /= 10;
	str_temperature[3] = temperature1 % 10 + '0'; // ,0
	temperature1 /= 10;
	str_temperature[2] = '.';
	str_temperature[1] = temperature1 % 10 + '0'; // 0,
	temperature1 /= 10;
	str_temperature[0] = temperature1 % 10 + '0'; // 00,
	str_temperature[5] = ' ';
	str_temperature[6] = 'C';
	str_temperature[7] = '\0';

	SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T1_POS);
	SSD1306_Puts (str_temperature, &Font_7x10, 1);
	HAL_Delay(1); // For display



	temp_buf = temperature2 / 10;

	str_temperature[4] = temperature2 % 10 + '0'; // ,00
	temperature2 /= 10;
	str_temperature[3] = temperature2 % 10 + '0'; // ,0
	temperature2 /= 10;
	str_temperature[1] = temperature2 % 10 + '0'; // 0,
	temperature2 /= 10;
	str_temperature[0] = temperature2 % 10 + '0'; // 00,

	SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T2_POS);
	SSD1306_Puts (str_temperature, &Font_7x10, 1);
	HAL_Delay(1); // For display

	SSD1306_UpdateScreen();
	HAL_Delay(5); // For display
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
