#include "SWM2X1.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN2, 0, 1, 0, 0);			//���룬����ʹ�ܣ���KEY
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//����� ��LED
	
	while(1==1)
	{
		if(GPIO_GetBit(GPIOA, PIN2) == 0)		//����������
		{
			GPIO_SetBit(GPIOA, PIN5);
		}
		else
		{
			GPIO_ClrBit(GPIOA, PIN5);
		}
	}
}
