#include "SWM2X1.h"

RTC_AlarmStructure RTC_AlarmStruct;

int main(void)
{
	uint32_t i;
	RTC_InitStructure RTC_initStruct;
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN6, 1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);					//�� LED��ָʾ����ִ��״̬
	GPIO_SetBit(GPIOA, PIN5);							//����LED
	for(i = 0; i < SystemCoreClock/2; i++) __NOP();		//��ʱ����ֹ�ϵ��SWD�����е��޷����س���
	
	RTC_initStruct.clksrc = RTC_CLKSRC_LRC32K;
	RTC_initStruct.Year = 2016;
	RTC_initStruct.Month = 5;
	RTC_initStruct.Date = 5;
	RTC_initStruct.Hour = 15;
	RTC_initStruct.Minute = 5;
	RTC_initStruct.Second = 5;
	RTC_initStruct.SecondIEn = 0;
	RTC_initStruct.MinuteIEn = 0;
	RTC_Init(RTC, &RTC_initStruct);
	
	RTC_AlarmStruct.Days = RTC_SUN | RTC_MON | RTC_TUE | RTC_WED | RTC_THU | RTC_FRI | RTC_SAT;
	RTC_AlarmStruct.Hour = RTC_initStruct.Hour;
	RTC_AlarmStruct.Minute = RTC_initStruct.Minute;
	RTC_AlarmStruct.Second = RTC_initStruct.Second + 5;
	RTC_AlarmStruct.AlarmIEn = 1;
	RTC_AlarmSetup(RTC, &RTC_AlarmStruct);
	
	RTC_Start(RTC);
	
	SYS->RTCWKCR = (1 << SYS_RTCWKCR_EN_Pos);
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//����LED
		for(i = 0; i < SystemCoreClock/5; i++);
		GPIO_ClrBit(GPIOA, PIN5);					//Ϩ��LED
		
		EnterSleepMode();
		
		while((SYS->RTCWKSR & (1 << SYS_RTCWKSR_FLAG_Pos)) == 0);	//�ȴ���������
		SYS->RTCWKSR |= (1 << SYS_RTCWKSR_FLAG_Pos);				//�������״̬
	}
}


void RTC_Handler(void)
{
	RTC_DateTime dateTime;
	
	if(RTC_INTStat(RTC, RTC_IT_SECOND))
	{
		RTC_INTClr(RTC, RTC_IT_SECOND);
	}
	
	if(RTC_INTStat(RTC, RTC_IT_MINUTE))
	{
		RTC_INTClr(RTC, RTC_IT_MINUTE);
	}
	
	if(RTC_INTStat(RTC, RTC_IT_ALARM))
	{
		RTC_INTClr(RTC, RTC_IT_ALARM);
		
		RTC_GetDateTime(RTC, &dateTime);
		if(dateTime.Second < 55)
		{
			RTC_AlarmStruct.Second = dateTime.Second + 5;
		}
		else
		{
			RTC_AlarmStruct.Second = 0;
			RTC_AlarmStruct.Minute = dateTime.Minute + 1;
		}
		RTC_AlarmSetup(RTC, &RTC_AlarmStruct);
		
		GPIO_InvBit(GPIOA, PIN6);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	//GPIOA.0����ΪUART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);	//GPIOA.1����ΪUART0 TXD
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* ��������: fputc()
* ����˵��: printf()ʹ�ô˺������ʵ�ʵĴ��ڴ�ӡ����
* ��    ��: int ch		Ҫ��ӡ���ַ�
*			FILE *f		�ļ����
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
