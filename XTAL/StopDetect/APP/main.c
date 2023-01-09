#include "SWM2X1.h"

void SerialInit(void);

int main(void)
{	
	int i;
	
	SystemInit();
	
	SerialInit();
	
	for(i = 0; i < SystemCoreClock/4; i++) __NOP();	//�ȴ������ȶ�����ֹ�ϵ�ʱ��ʶ����ͣ��
	
	SYS->XTALSR = SYS_XTALSR_STOP_Msk;				//�����־
	NVIC_ClearPendingIRQ(XTALSTOP_IRQn);
	NVIC_EnableIRQ(XTALSTOP_IRQn);
   	
	while(1==1)
	{
		printf("SystemCoreClock = %d\r\n", SystemCoreClock);
		
		for(i = 0; i < SystemCoreClock/4; i++) __NOP();
	}
}


void XTALSTOP_Handler(void)
{
	/* ����ִ�� switchTo12MHz()������ָ���ʱϵͳʱ�ӻ��Զ��л����ⲿ����
	   ���ⲿ�����ȶ������񵴺Ͳ��񵴼������л�����ϵͳʱ��Ҳ�����ڲ�ʱ�Ӻ��ⲿʱ��֮�������л� */
#if defined(CHIP_SWM201)
	switchTo30MHz();
#elif defined(CHIP_SWM211)
	switchTo12MHz();
#endif
	SystemCoreClockUpdate();
	
	SYS->XTALCR = 0;							//�ⲿ���������������ر�
	NVIC_DisableIRQ(XTALSTOP_IRQn);
	
	UART_SetBaudrate(UART0, 57600);				//ϵͳ��Ƶ�仯���������趨������
	printf("XTAL Stop Detected!\r\n");
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
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
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
