#include "SWM2X1.h"


int main(void)
{	
	uint32_t i;
	SystemInit();
	
	GPIO_Init(GPIOA, PIN3, 1, 0, 0, 0);					//ģ���ⲿ�������ź�
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);					//GPIOA.5 => LED
	
	PORT_Init(PORTA, PIN14, PORTA_PIN14_TIMR0_IN, 1);	//�����ⲿ�������źţ�PA3 => PA14
	PORTA->PULLU |= (1 << PIN14);
	
	TIMR_Init(TIMR0, TIMR_MODE_COUNTER, 1, 3, 1);		//ÿ��3�������ؽ����ж�
	TIMR_Start(TIMR0);
	
	while(1==1)
	{
		GPIO_InvBit(GPIOA, PIN3);
		for(i = 0; i < CyclesPerUs*1000; i++) __NOP();
	}
}

void TIMR0_Handler(void)
{
	TIMR_INTClr(TIMR0);
	
	GPIO_InvBit(GPIOA, PIN5);
}
