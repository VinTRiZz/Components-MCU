/*
TODO list

// ШИМ на одном канале одной частоты, надо разную. Два таймера?

// ssd1306.c Переписать HAL-ы

1) Заменить ф-и инициализации

2) Заменить ф-и интерфейсов

3) Заменить ф-и в библиотеке для драйвера (I2C)

4) Проверить каким-нибудь образом (например, протеус)



*/

#ifdef I2C_FUNC
// Переписать вот это под AVR:

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
	/* Get tick */
	uint32_t tickstart = HAL_GetTick();
	uint32_t I2C_Trials = 0U;
	FlagStatus tmp1;
	FlagStatus tmp2;

	if (hi2c->State == HAL_I2C_STATE_READY)
	{
		/* Wait until BUSY flag is reset */
		if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
		{
			return HAL_BUSY;
		}

		/* Process Locked */
		__HAL_LOCK(hi2c);

		/* Check if the I2C is already enabled */
		if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
		{
			/* Enable I2C peripheral */
			__HAL_I2C_ENABLE(hi2c);
		}

		/* Disable Pos */
		CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

		hi2c->State = HAL_I2C_STATE_BUSY;
		hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
		hi2c->XferOptions = I2C_NO_OPTION_FRAME;

		do
		{
			/* Generate Start */
			SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

			/* Wait until SB flag is set */
			if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, tickstart) != HAL_OK)
			{
				if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
				{
					hi2c->ErrorCode = HAL_I2C_WRONG_START;
				}
				return HAL_TIMEOUT;
			}

			/* Send slave address */
			hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

			/* Wait until ADDR or AF flag are set */
			/* Get tick */
			tickstart = HAL_GetTick();

			tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
			tmp2 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
			while ((hi2c->State != HAL_I2C_STATE_TIMEOUT) && (tmp1 == RESET) && (tmp2 == RESET))
			{
				if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
				{
					hi2c->State = HAL_I2C_STATE_TIMEOUT;
				}
				tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
				tmp2 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
			}

			hi2c->State = HAL_I2C_STATE_READY;

			/* Check if the ADDR flag has been set */
			if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)
			{
				/* Generate Stop */
				SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

				/* Clear ADDR Flag */
				__HAL_I2C_CLEAR_ADDRFLAG(hi2c);

				/* Wait until BUSY flag is reset */
				if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
				{
					return HAL_ERROR;
				}

				hi2c->State = HAL_I2C_STATE_READY;

				/* Process Unlocked */
				__HAL_UNLOCK(hi2c);

				return HAL_OK;
			}
			else
			{
				/* Generate Stop */
				SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

				/* Clear AF Flag */
				__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

				/* Wait until BUSY flag is reset */
				if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
				{
					return HAL_ERROR;
				}
			}

			/* Increment Trials */
			I2C_Trials++;
		}
		while (I2C_Trials < Trials);

		hi2c->State = HAL_I2C_STATE_READY;

		/* Process Unlocked */
		__HAL_UNLOCK(hi2c);

		return HAL_ERROR;
	}
	else
	{
		return HAL_BUSY;
	}
}
#endif // I2C_FUNC


/* Includes ------------------------------------------------------------------*/
#include "ssd1306.h"
#include "fonts.h"
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <interrupt.h>
#include <stdint.h>


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

// Maximum pulse, that can be in PWM generators
#define MAXIMUM_PULSE 127

// Temperature limits of temp resistors
#define T_MAX 100
#define T_MIN 30

// Resistor in a voltage divider, Om
#define Rb 20000.0f
// Voltage in a voltage divider input, V
#define Ub 5.0f
// Reference voltage
#define Vref 1.1f


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);

void ADC_PollForConversion();
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
int8_t changePulse_ch1(int8_t value);
int8_t changePulse_ch2(int8_t value);

int16_t adc[3] = {0};
int8_t adc_ch = 0;
int16_t res = 0;
int8_t PRESET = 0;

ISR(ADC_vect)
{
	switch (adc_ch)
	{
		case 0:
		adc[0] = 512 * ADCH + ADCL;
		break;

		case 1:
		adc[1] = 512 * ADCH + ADCL;
		break;

		case 2:
		adc[2] = 512 * ADCH + ADCL;
		break;

		default:
		adc_ch = 0;
		adc[0] = 512 * ADCH + ADCL;
	}
	adc_ch++;
}

ISR(TIMER0_OVF_vect)
{
	TIMSK0 = PRESET;
	// Toggle pins
	if (PIND % 40)
		PORTD &= ~(0b11000000);
	else
		PORTD |= 0b11000000;
}



int main(void)
{
	sei();
	
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  
  // ----------------Display:
  SSD1306_Init();
  _delay_ms(5);

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

  while (1)
  {
		// Activate or deactivate PWM1
		if (PIND & ~0b11111110)
		{
	  		PWM1_ON = !PWM1_ON;
	  		if (PWM1_ON)
	  		{
	  			DDRB |= 0b10000000;
	  			SSD1306_ClearData(1);
	  			SSD1306_PWM_SEND(pwm_ch1,0);
	  		}
	  		else
	  		{
	  			DDRB &= ~(0b10000000);
	  			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
				SSD1306_Puts("Выкл.",&Font_7x10,1);
	  		}

	  		while (PIND & ~0b11111110)
	  			_delay_ms(100);
		}

		// Activate or deactivate PWM2
	  	if (PIND & ~0b11111101)
	  	{
			PWM2_ON = !PWM2_ON;
			if (PWM2_ON)
			{
				DDRB |= 0b01000000;
				SSD1306_ClearData(2);
				SSD1306_PWM_SEND(pwm_ch2,1);
			}
			else
			{
				DDRB &= ~(0b11000000);
				SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
				SSD1306_Puts("Выкл.",&Font_7x10,1);
			}


			while (PIND & ~0b11111101)
				_delay_ms(100);
	  	}

	  	if (PWM1_ON)
	  	{
			// Change pulse
			if (PIND & ~0b11111011)
			{
			  if (pwm_ch1 < 100)
				  pwm_ch1++;
			  else
				  pwm_ch1 = 100;
			  changePulse_ch1(pwm_ch1);
			  SSD1306_ClearData(1);
			  SSD1306_PWM_SEND(pwm_ch1,0);
			  _delay_ms(100);
			  while (PIND & ~0b11111011) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch1 < 99)
					  pwm_ch1+=2;
				  else
					  pwm_ch1 = 100;
				  changePulse_ch1(pwm_ch1);
				  SSD1306_ClearData(1);
				  SSD1306_PWM_SEND(pwm_ch1,0);
				  _delay_ms(20);
			  }
			}

			if (PIND & ~0b11110111)
			{
			  if (pwm_ch1 > 0)
				  pwm_ch1--;
			  else
				  pwm_ch1 = 0;
			  changePulse_ch1(pwm_ch1);
			  SSD1306_ClearData(1);
			  SSD1306_PWM_SEND(pwm_ch1,0);
			  _delay_ms(1000);
			  while (PIND & ~0b11110111) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch1 > 1)
					  pwm_ch1-=2;
				  else
					  pwm_ch1 = 0;
				  changePulse_ch1(pwm_ch1);
				  SSD1306_ClearData(1);
				  SSD1306_PWM_SEND(pwm_ch1,0);
				  _delay_ms(20);
			  }
			}
	  	}

	  	if (PWM2_ON)
	  	{
			if (PIND & ~0b11101111)
			{
			  if (pwm_ch2 < 100)
				  pwm_ch2++;
			  else
				  pwm_ch2 = 100;
			  changePulse_ch2(pwm_ch2);
			  SSD1306_ClearData(2);
			  SSD1306_PWM_SEND(pwm_ch2,1);
			  _delay_ms(1000);
			  while (PIND & ~0b11101111) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch2 < 99)
					  pwm_ch2+=2;
				  else
					  pwm_ch2 = 100;
				  changePulse_ch2(pwm_ch2);
				  SSD1306_ClearData(2);
				  SSD1306_PWM_SEND(pwm_ch2,1);
				  _delay_ms(20);
			  }
			}

			if (PIND & ~0b11101111)
			{
			  if (pwm_ch2 > 0)
				  pwm_ch2--;
			  else
				  pwm_ch2 = 0;
			  changePulse_ch2(pwm_ch2);
			  SSD1306_ClearData(2);
			  SSD1306_PWM_SEND(pwm_ch2,1);
			  _delay_ms(1000);
			  while (PIND & ~0b11101111) // Ускорение ввода если кнопка не отпущена
			  {
				  if (pwm_ch2 > 1)
					  pwm_ch2-=2;
				  else
					  pwm_ch2 = 0;
				  changePulse_ch2(pwm_ch2);
				  SSD1306_ClearData(2);
				  SSD1306_PWM_SEND(pwm_ch2,1);
				  _delay_ms(20);
			  }
			}
	  	}

		// Get data from ADC
		// INPUT VOLTAGE = (ADC Value / ADC Resolution) * Reference Voltage
	  	// For ask if prev value was diff to new
	  	buf_adc_temp1 = adc_temp1;
	  	buf_adc_temp2 = adc_temp2;
	  	// Get new values
		ADC_PollForConversion();
		
		adc_battery = ((float)adc[0]*Vref) / 1024.0f;
		adc_temp1 = getTemperature(0);
		adc_temp2 = getTemperature(1);

		_delay_ms(1); // For display

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
		_delay_ms(50);
  }
}

void MX_ADC1_Init(void)
{
	ADCSRA = 0;
	// Enable ADC
	ADCSRA |= (1<<ADEN);
	// Enable interruts
	ADCSRA |= (1<<ADIE);
	// Prescaler is 2 (don't actually matter)
	ADCSRA |= (1 << ADPS0);
	
	ADMUX = 0;
	// Enable 1.1 Vref with capacitor on AREF pin
	ADMUX |= (1 << REFS0) | (1 << REFS1);
	
	// ADC = Vin * 1024 / Vref
	
	// Start conversion
	ADCSRA |= (1<<ADSC);
}

static void MX_TIM1_Init(void)
{
	/*
	Init timer to PWM mode
	2 channels
	*/
	
	// Phase correct mode
	// f = fclk / (prescaler * 510)
	
	TCCR0A = 0;
	// Phase correct PWM
	TCCR0A |= (1<<WGM00);
	
	TCCR0B = 0;
	// No prescaler
	TCCR0B |= (1<<CS00);
	
}

void MX_GPIO_Init(void)
{
	/*
	init 6 pins IN:
	2 but +
	2 but -
	2 but ON/OFF
	
	init 3 pins analog IN:
	1 bat
	2 temperature
	*/
	
	// Buttons init
	DDRB &= ~(0b111111); // First 6 will be buttons
}

void ADC_PollForConversion()
{
	// Choose channel
	ADMUX &= ~0b0000;
	ADMUX |= 0b0000;
	
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); // Wait for conversation on a channel
	adc[0] = ADCH * 512 + ADCL;
	
	// Change channel
	ADMUX &= ~0b0000;
	ADMUX |= 0b0001;
	
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); // Wait for conversation on a channel
	adc[1] = ADCH * 512 + ADCL;
	
	// Change channel
	ADMUX &= ~0b0000;
	ADMUX |= 0b0010;
	
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); // Wait for conversation on a channel
	adc[2] = ADCH * 512 + ADCL;
}


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
				_delay_ms(1);
			}
		break;

	case 2: // PWM 2
		SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
		for (int i = PWM_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
			{
				SSD1306_Putc(' ',&Font_7x10,1);
				_delay_ms(1);
			}
		SSD1306_UpdateScreen();
		break;

	case 3: // TEMPERATURE 1
		SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T1_POS);
		for (int i = TEMPERATURE_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
			{
				SSD1306_Putc(' ',&Font_7x10,1);
				_delay_ms(1);
			}
		SSD1306_UpdateScreen();
		break;

	case 4: // TEMPERATURE 2
		SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T2_POS);
		for (int i = TEMPERATURE_DATA_CLEAR; i < SCREEN_WIDTH; i++)
			if (!(i % 7 - 3))
				{
					SSD1306_Putc(' ',&Font_7x10,1);
					_delay_ms(1);
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
	_delay_ms(1); // For display
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
			_delay_ms(1); // For display
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
			_delay_ms(1); // For display
			SSD1306_Putc ('%', &Font_7x10, 1);
			_delay_ms(1); // For display
		}
	}
	else
		if (!channel)
			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM1_POS);
		else
		{
			SSD1306_GotoXY(SCREEN_WIDTH - PWM_DATA_CLEAR,PWM2_POS);
			SSD1306_Puts("0%", &Font_7x10, 1);
			_delay_ms(1); // For display
		}

    SSD1306_UpdateScreen();
    _delay_ms(1); // For display
}

/************************************************************************/
/* Changes pulse value of channel 1                                     */
/************************************************************************/
int8_t changePulse_ch1(int8_t value) // For ch 1 tim 1
{
	DDRB &= ~0b10000000; // Disable PWM1
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;
	// now 0 <= value <= 100
	
	PRESET = ((float)value / 100.0f) * MAXIMUM_PULSE;
	
	DDRB |= 0b10000000; // Enable PWM1
}

/************************************************************************/
/* Changes pulse value of channel 2                                     */
/************************************************************************/
int8_t changePulse_ch2(int8_t value) // For ch 1 tim 1
{
	DDRB &= ~0b01000000; // Disable PWM2
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;
	// now 0 <= value <= 100

	PRESET = ((float)value / 100.0f) * MAXIMUM_PULSE;
	
	DDRB |= 0b10000000; // Enable PWM2
}

/**
 * @brief
 * @param T Temperature to show, 0 for first and 1 to second
 * @retval Temperature value. Example: T=31.7; retval=317
 */
int16_t getTemperature(int8_t T)
{
	// T( R ) == -22.54 * ln( R ) + 273.81 // Approximated curve
	float buf = 0;
	if (T)
	{
		buf = ((float)adc[1]*Vref) / 1024.0f;
		buf = (5.0f - buf) * Rb / (buf * 1.0f);
		buf = -22.54f * log(buf*1.0f) + 273.81f;
	}
	else
	{
		buf = ((float)adc[2]*Vref) / 1024.0f;
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
	_delay_ms(1); // For display

	str_temperature[4] = temperature2 % 10 + '0'; // ,00
	temperature2 /= 10;
	str_temperature[3] = temperature2 % 10 + '0'; // ,0
	temperature2 /= 10;
	str_temperature[1] = temperature2 % 10 + '0'; // 0,
	temperature2 /= 10;
	str_temperature[0] = temperature2 % 10 + '0'; // 00,

	SSD1306_GotoXY(SCREEN_WIDTH - TEMPERATURE_DATA_CLEAR,T2_POS);
	SSD1306_Puts (str_temperature, &Font_7x10, 1);
	_delay_ms(1); // For display

	SSD1306_UpdateScreen();
	_delay_ms(5); // For display
}