#include "SWM2X1.h"


/* ע�⣺
 *	оƬ�� ISP��SWD ����Ĭ�Ͽ������������裬���������߹��ģ�������������߹��ģ�����ǰ��ر��������ŵ���������������
 */
 
 
int main(void)
{
	uint32_t i;
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);						//�� LED��ָʾ����ִ��״̬
	GPIO_SetBit(GPIOA, PIN5);								//����LED
	for(i = 0; i < SystemCoreClock/2; i++) __NOP();			//��ʱ����ֹ�ϵ��SWD�����е��޷����س���
	
	GPIO_Init(GPIOA, PIN4, 0, 1, 0, 0);						//�Ӱ���������ʹ��
	SYS->PAWKEN |= (1 << PIN4);								//����PA4���ŵ͵�ƽ����
	
	SYS->LRCCR |= 0x01;										//����ģʽʹ�õ�Ƶʱ��
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);							//����LED
		for(i = 0; i < SystemCoreClock/8; i++) __NOP();
		GPIO_ClrBit(GPIOA, PIN5);							//Ϩ��LED
		
#if defined(CHIP_SWM211)
		switchTo12MHz();	//����ǰ�л����ڲ�RCʱ��
#endif
		
		EnterSleepMode();
		
		while((SYS->PAWKSR & (1 << PIN4)) == 0) __NOP();	//�ȴ���������
		SYS->PAWKSR |= (1 << PIN4);							//�������״̬
		
#if defined(CHIP_SWM211)
		switchToPLL(0);		//���Ѻ��л���PLLʱ��
#endif
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
