#define F_CPU 1000000UL

#define Vref 2.56
#define GAIN 10

/////////////////////////////////////////////////////////////////
/////// Ниже копирован код из лабораторной работы ///////////////
/////////////////////////////////////////////////////////////////

/* макроопределение, для работы с битами */

#include <avr/io.h>
#include <avr/interrupt.h>

#define BIT(n) (1<<(n))
#define ENABLE(x,n) ((x) |= BIT(n))
#define CHECKBIT(x,n) ((x) & BIT(n))
char ADCdata;  // глобальная переменная для хранения измеренных данных
short int times = 0; // Для усреднения
float dif_result = 0; // Разница напряжений на входах диф порта

void port_init(void)
{
	PORTA = 0x00;
	DDRA = 0x00; // порт А делаем входным
	PORTB = 0x00; // остальные порты не трогаем
	DDRB = 0x00;
	PORTC = 0x00; // А зачем тогда их писать вообще?
	DDRC = 0x00; // Они же не подключены ни к чему и 
	PORTD = 0x00; // не повлияют на работу МК
	DDRD = 1; // lamp
}

// ADC initialize // Conversion time: 104uS
void adc_init(void)
{ 
	
	ADCSRA = 0x00; // disable adc and all flags
	
	// Дифференциальным входом будут входы ADC0 и ADC1, где 0 это минус и 1 это плюс
	// Для этого надо разрешить его к использованию
	
	 // Работа с 0 пином как минус, 1 как плюс и отключение остальных. GAIN == 10x
	ADMUX |= ~( (1<<MUX4) | (1<<MUX2) | (1<<MUX1) ) | (1<<MUX3) | (1<<MUX0) | (1<<ADLAR);
	
	ADMUX |= (1<<REFS1) | (1<<REFS0); // Установка Vref как 2,56 вольт с внешним источником на AREF пине
	
	ADCSRA |= (1<<ADSC); // Начало преобразований ADC
	
	ACSR = 0x80; // выключаем аналоговый компаратор
	
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS0);
	/* включаем прерывание по окончанию преобразования, 
	устанавливаем частоту преобразования */
}

/*ISR(ADC_vect) // До модификации (пункта "Задание 2")
{
	ADCdata = ADCH; // отсылаем данные
	ENABLE(ADCSRA, ADSC); // запускаем новое преобразование
}
*/

ISR(ADC_vect) // После модификации (пункта "Задание 2")
{
		// Формула значения ADC:
		/*
				(Vpos - Vneg) * GAIN * 512
		ADC =	---------------------------
							Vref
		*/
		// Следовательно, разница напряжений будет вычисляться как:
		/*
						ADC * Vref
		(Vpos - Vneg) = ----------
						GAIN * 512
		*/
	PORTD = 1; // indicate lamp
	if (times < 4) {
	dif_result += (ADC * Vref) / (GAIN * 512); // Запоминание нового результата измерения
	times++;
	ENABLE(ADCSRA, ADSC); // запускаем новое преобразование
	PORTD = 0; // lamp disabled while not ended
	}
	else // if (times > 3)
	{
		times = 0;
		while( !CHECKBIT(UCSRA,UDRE) ); // ждем, пока освободится буфер
			UDR = (dif_result / 4); // отсылаем усреднённые данные
	}
}

void init_devices(void)
{
	cli(); // на время инициализации периферии запрещаем все прерывания
	port_init();
	adc_init();
	MCUCR = 0x00; // обнуляем на всякий случай все остальные прерывания
	GICR = 0x00;
	TIMSK = 0x00; // timer interrupt sources
	sei(); // разрешаем обратно все прерывания
}

// USART initialize
// desired baud rate: 9600 actual: baud rate: 9615 (0,2%)
// char size: 8 bit parity: Disabled
void usart_init(unsigned baudrate)
{
	UCSRB = 0x00; // disable while setting baud rate
	UCSRA = 0x00;
	
	// Set this bit 1 will increase in ASYNCHRONOUS mode speed by 2x
	UCSRA |= 1 << U2X;
	
	UCSRC = BIT(URSEL) | 0x06;
	UBRRL = baudrate;
	UCSRB = 0x08;
}

/////////////////////////////////////////////////////////////////
////// Далее -- изменённая функция main(void) ///////////////////
/////////////////////////////////////////////////////////////////

int main(void)
{
	init_devices();
	usart_init(25); // For read assembly command
	while(1)
	{
		// Считаем, что кнопка подключена к порту D на пин 1
		// Тогда при нажатии начнутся преобразования и их передача
		if (PIND1 == 1)
			ADCSRA |= (1<<ADSC); // Start conversion and send
	}
}
