#include "SWM2X1.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//�������LED
	
	GPIO_Init(GPIOA, PIN2, 0, 1, 0, 0);			//���룬����ʹ�ܣ���KEY
	
	EXTI_Init(GPIOA, PIN2, EXTI_FALL_EDGE);		//�½��ش����ж�
	
	NVIC_EnableIRQ(GPIOA2_GPIOM2_IRQn);
	
	EXTI_Open(GPIOA, PIN2);
	
	while(1==1)
	{
	}
}

void GPIOA2_GPIOM2_Handler(void)
{
	EXTI_Clear(GPIOA, PIN2);
	
	GPIO_InvBit(GPIOA, PIN5);
}
