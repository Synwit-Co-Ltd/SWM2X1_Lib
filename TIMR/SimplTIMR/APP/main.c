#include "SWM2X1.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN4, 1, 0, 0, 0);			//�������LED
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//�������LED
	
	TIMR_Init(TIMR0, TIMR_MODE_TIMER, CyclesPerUs, 500000, 1);		//ÿ0.5���Ӵ���һ���ж�
	TIMR_Start(TIMR0);
	
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, CyclesPerUs, 500000, 1);		//ÿ0.5���Ӵ���һ���ж�
	TIMR_Start(BTIMR0);
	
	while(1==1)
	{
	}
}

void TIMR0_Handler(void)
{
	TIMR_INTClr(TIMR0);
	
	GPIO_InvBit(GPIOA, PIN4);	//��תLED����״̬
}

void BTIMR0_Handler(void)
{	
	TIMR_INTClr(BTIMR0);
	
	GPIO_InvBit(GPIOA, PIN5);	//��תLED����״̬
}
