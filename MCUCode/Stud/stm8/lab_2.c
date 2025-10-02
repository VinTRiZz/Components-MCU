#include "iostm8s003k3.h" 
int times = 0;
int i = 0;
int MODE_VAL = 100;
__interrupt void TIM1_OVR_UIF(void);
int main (void)
{
  PD_DDR_bit.DDR0=1; // out
  PD_CR1_bit.C10 = 1; // pushpull out
  PD_CR2_bit.C20 = 0; // change count 2mhz
  PD_ODR_bit.ODR0= 1; // light down
  CLK_PCKENR2=0xff; // tact
  TIM1_CR1_bit.URS=1; // overflow inter 
  TIM1_IER_bit.UIE=1; // inter enable
  TIM1_PSCRL = 0x0f; // prescale to 1000 hz
  TIM1_PSCRH = 0x27;
  // count up to 10 (10 ms) and reset 
  TIM1_ARRL = 0x9;
  TIM1_CR1_bit.CEN=1; // start timer
  while (1)
  {
    if (times < 100)
      if (times < 49)  // fade
      {
        times += i;
        PD_ODR_bit.ODR0= 0; // light up
        i++;
      }
      else
      {
        PD_ODR_bit.ODR0= 1; // light down
        i = 0;
      }
    else
    if (times < 200) // enlight
      if (times < 49)  // fade
      {
        PD_ODR_bit.ODR0= 0; // light up
        i = 0;
      }
      else
      {
        times += i;
        PD_ODR_bit.ODR0= 1; // light down
        i++;
      }
    if (times > 200)
      times = 0;
  }
}
__interrupt void TIM1_OVR_UIF(void)
{
TIM1_SR1_UIF = 0;
times++;	TIM1_CR1_bit.CEN = 1; }
