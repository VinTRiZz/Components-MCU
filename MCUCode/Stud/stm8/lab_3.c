/*

!!! ATTENTION !!!

ADC actually don't work as expected

*/
#include "iostm8s003k3.h" 

// ADC VALUES
int ADC_RES = 0;
int ADC_BASE_RIGHT = 0;
int ADC_BASE_LEFT = 0;
int times_checked = 0;


int time = 0; // ticks of timer

#pragma vector=0x18
__interrupt void ADC_end(void); // ADC vect
void blink(int time);
void merc(int time); // one time is 10 ms

int main (void)
{
  
  // portd
  PD_DDR_bit.DDR0=1; // out
  PD_CR1_bit.C10 = 1; // pushpull out
  PD_CR2_bit.C20 = 0; // change count 2mhz
  PD_ODR_bit.ODR0= 1; // light down
  
  PC_DDR_bit.DDR7=1; // out
  PC_CR1_bit.C10 = 1; // pushpull out
  PC_CR2_bit.C20 = 0; // change count 2mhz
  PC_ODR_bit.ODR7= 0; // light up
  
  
  // ADC
  CLK_PCKENR2=0xff; // ADC tact
  ADC_CSR_bit.AWD=0; // WD decline
  ADC_CSR_bit.EOCIE=1; // inter on end en
  ADC_CSR_bit.AWDIE=0; // inter on WD off
ADC_CSR_bit.CH=0x0;  // ADC0
ADC_CR1_bit.SPSEL=0x0; 
ADC_CR1_bit.CONT=0;  
ADC_CR2_bit.EXTTRIG=0;
ADC_CR2_bit.ALIGN= 1;
ADC_CR3_bit.DBUF=0;  
ADC_CR3_bit.OVR=0;  
ADC_CR1_bit.ADON=1; 
  

// timer 1
CLK_PCKENR2=0xff; // tact
  TIM1_CR1_bit.URS=1; // overflow inter 
  TIM1_IER_bit.UIE=1; // inter enable
  TIM1_PSCRL = 0x0f; // prescale to 1000 hz
  TIM1_PSCRH = 0x27;
  // count up to 10 (10 ms) and reset 
  TIM1_ARRL = 0x9;
  TIM1_CR1_bit.CEN=1; // start timer
  
  
  asm("rim");
  while (1)
  {
    blink_and_fade();
  }
}


__interrupt void TIM1_OVR_UIF(void)
{
TIM1_SR1_UIF = 0;

times++;
if (time > 5000) // max time count -- 5 sec
  time = 0;

TIM1_CR1_bit.CEN = 1;
}



// need time, maxTime, blinkTime
void blink_and_fade(void)
{
while (time != 0) // start blink when time is 0, no error comp
asm("nop");
int bufTime = 0;

// fade block
for (int i = 0; i < maxTime; i++)
{
PD_ODR_bit.ODR0 = 0; // light up
}
for (int i = 0; i < (1000 - maxTime); i++)
{
PD_ODR_bit.ODR0 = 1; // light down
}

// blink time interval
while (time < blinkTime * 200) // every blinkTime is 0,2 sec
asm("nop);

}
// 0 < maxTime < 500 // Zero obvious, 500 max time because... Because.
// 5 < blinkTime < 25 // more 5 in case of 1000 ms cycle fade


__interrupt void ADC_end(void)
{
  ADC_CSR_bit.EOC=0;
  
ADC_RES = ADC_DRH * 128 + ADC_DRL; // check data

// remember 1st value
  if (adc_mode == 0)
    {
     adc_mode++;
     ADC_BASE_RIGHT = ADC_RES;
    }
  else
    if (adc_mode == 1)
    {
      adc_mode++; // moment 1 -- adc_mode is 2 when comparison is passed
      ADC_BASE_LEFT = ADC_RES;
    }

// change values of times
if (adc_mode > 1) // if base values have got
	if (times_checked) // 1st, is left because of moment 1
		if (ADC_BASE_LEFT > ADC_RES)
			if (maxTime > 0)
				maxTime--; // blink fade time
			else
				maxTime = 0; // err preciever
		else if (ADC_BASE_LEFT < ADC_RES)
			if (maxTime < 500)
				maxTime++; // blink fade time
			else
				maxTime = 500; // err preciever
	else // times checked
		if (ADC_BASE_RIGHT > ADC_RES)
			if (blinkTime > 5)
				blinkTime--; // blink fade time
			else
				blinkTime = 5; // err preciever
		else
			if (blinkTime < 25)
				blinkTime++; // blink fade time

			else
				blinkTime = 25; // err preciever

// 0 < maxTime < 500 // Zero obvious, 500 max time because... Because.
// 5 < blinkTime < 25 // more 5 in case of 1000 ms cycle fade
ADC_CR1_bit.ADON=1;
}
