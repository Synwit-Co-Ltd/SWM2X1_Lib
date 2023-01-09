#include "SWM2X1.h"


int main(void)
{
	uint32_t i;
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
		
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PORT_Init(PORTA, PIN5, PORTA_PIN5_PWM1A,  0);
	PORT_Init(PORTM, PIN4, PORTM_PIN4_PWM1AN, 0);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_PWM1B,  0);
	PORT_Init(PORTM, PIN5, PORTM_PIN5_PWM1BN, 0);
	
	PWM_initStruct.Mode = PWM_ASYM_CENTER_ALIGNED;
	PWM_initStruct.Clkdiv = 6;					//F_PWM = 60M/6 = 10M
	PWM_initStruct.Period = 10000;				//10M/(10000 + 10000) = 500Hz
	PWM_initStruct.HdutyA =  2500;				//(2500+2500)/(10000+10000) = 25%
	PWM_initStruct.HdutyA2 = 2500;
	PWM_initStruct.DeadzoneA = 50;
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB =  7500;				//(7500+7500)/(10000+10000) = 75%
	PWM_initStruct.HdutyB2 = 7500;
	PWM_initStruct.DeadzoneB = 5;
	PWM_initStruct.IdleLevelB = 0;
	PWM_initStruct.IdleLevelBN= 0;
	PWM_initStruct.OutputInvB = 0;
	PWM_initStruct.OutputInvBN= 0;
	PWM_initStruct.UpOvfIE    = 0;
	PWM_initStruct.DownOvfIE  = 0;
	PWM_initStruct.UpCmpAIE   = 0;
	PWM_initStruct.DownCmpAIE = 0;
	PWM_initStruct.UpCmpBIE   = 0;
	PWM_initStruct.DownCmpBIE = 0;
	PWM_Init(PWM0, &PWM_initStruct);
	PWM_Init(PWM1, &PWM_initStruct);
	
	PWM_Start(PWM0_MSK | PWM1_MSK);
	
	while(1==1)
	{
		PWMG->RELOADEN = 0x00;		// �رչ����Ĵ������ظ��£���֤���мĴ�������������ٸ����������
		
		/* ʹ�÷ǶԳ����Ķ���ģʽ�������������ռ�ձȲ��䣬ʵ�����๦�� */
#if 0
		if(PWM1->CMPA2 == 0)
		{
			PWM1->CMPA = 2500;
			PWM1->CMPA2 = 2500;
			PWM1->CMPB = 7500;
			PWM1->CMPB2 = 7500;
		}
		else
		{
			PWM1->CMPA += 250;		// ǰ�����ڵĸߵ�ƽʱ������
			PWM1->CMPA2 -= 250;		// ������ڵĸߵ�ƽʱ����С���ߵ�ƽ��������
			PWM1->CMPB += 750;		// ע�⣺����˵��ǰ�����ڡ������������� PWM �������ļ�������˵�ģ����ϼ�������ǰ�����ڣ����¼��������������
			PWM1->CMPB2 -= 750;		// ��PWM����Ĳ����Ͽ���ǰ�����ڷ�����Ӧ PWM �ߵ�ƽ���Ҳಿ�֣�������ڷ�����Ӧ PWM �ߵ�ƽ����ಿ��
		}
#else
		if(PWM1->CMPA == 0)
		{
			PWM1->CMPA = 2500;
			PWM1->CMPA2 = 2500;
			PWM1->CMPB = 7500;
			PWM1->CMPB2 = 7500;
		}
		else
		{
			PWM1->CMPA -= 250;		// ǰ�����ڵĸߵ�ƽʱ������
			PWM1->CMPA2 += 250;		// ������ڵĸߵ�ƽʱ����С���ߵ�ƽ��������
			PWM1->CMPB -= 750;
			PWM1->CMPB2 += 750;
		}
#endif
		PWMG->RELOADEN = 0x3F;
		
		for(i = 0; i < SystemCoreClock / 32; i++) __NOP();
	}
}

