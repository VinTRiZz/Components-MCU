#include "iostm8.h" 
int i,j,z; 
int z = 0;
void interrupt_init(void);
#pragma vector=0x06
__interrupt void EXTI_PB7(void); 
int main( void )
{
PB_DDR_bit.DDR7=0; 
PB_CR1_bit.C17 = 0; 
PB_CR2_bit.C27 = 1; 
PD_DDR_bit.DDR0=1; 
PD_CR1_bit.C10 = 1;
PD_CR2_bit.C20 = 0; 
PD_ODR_bit.ODR0= 0; 
interrupt_init();
for (;;)
{
if (z == 0)
{PD_ODR_bit.ODR0 =0x00;
for (i=0;i<=30000;i++){};
PD_ODR_bit.ODR0 =0x01;
for (i=0;i<=30000;i++){};
}
if (z == 1)
{PD_ODR_bit.ODR0 =0x00;
for (i=0;i<=15000;i++){};
PD_ODR_bit.ODR0 =0x01;
for (i=0;i<=15000;i++){};
}
if (z == 2)
{PD_ODR_bit.ODR0 =0x00;
for (i=0;i<=7500;i++){};
PD_ODR_bit.ODR0 =0x01;
for (i=0;i<=7500;i++){};
}
if (z == 3)
{
for (int a=0; a <=500; a++)
{
PD_ODR_bit.ODR0 =0x00;
for (int i=0; i<a; i++);
PD_ODR_bit.ODR0 =0x01;
for (int i=0; i<500-a; i++);
}
for (int a=500; a >=0; a--)
{
PD_ODR_bit.ODR0 =0x00;
for (int i=0; i<a; i++);
PD_ODR_bit.ODR0 =0x01;
for (int i=0; i<500-a; i++);
}
}
}


}

void interrupt_init(void)
{
EXTI_CR1|=0x08;
asm("rim");
}

__interrupt void EXTI_PB7(void)
{
if (z<3)
{
z = z + 1;
}
else
{ z = 0;}
for (i=0;i<=30000;i++);
{
for (j=0;j<=30000;j++);
{
}
}

}
