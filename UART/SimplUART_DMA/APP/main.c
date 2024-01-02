#include <string.h>
#include "SWM2X1.h"


const char TX_String[8][32] = {
	"TestString\r\n",
	"TestString1\r\n",
	"TestString12\r\n",
	"TestString123\r\n",
	"TestString1234\r\n",
	"TestString12345\r\n",
	"TestString123456\r\n",
	"TestString1234567\r\n",
};

#define RX_LEN	256					// �Ƽ��� 2 ���������ݣ��Ӷ���ȡ������ת���������㣬�ӿ������ٶ�
int  RX_Buffer[RX_LEN] = { 0 };		// DMA ���� UART ���յ������ݱ�������Ϊ��λ��������ܳ��������ݴ��� DMA �ڲ��޷����������������
char TX_Buffer[RX_LEN] = { 0 };


volatile uint32_t RX_Start = 0;


void SerialInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	// ���� PA4������ UART1 ���������ݣ�Ȼ��ԭ������
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 1;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
   	
	DMA_InitStructure DMA_initStruct;
	
	DMA_initStruct.Mode = DMA_MODE_CIRCLE;
	DMA_initStruct.Unit = DMA_UNIT_WORD;
	DMA_initStruct.Count = RX_LEN;
	DMA_initStruct.SrcAddr = (uint32_t)&UART0->DATA;
	DMA_initStruct.SrcAddrInc = 0;
	DMA_initStruct.DstAddr = (uint32_t)RX_Buffer;
	DMA_initStruct.DstAddrInc = 1;
	DMA_initStruct.Handshake = DMA_CH0_UART0RX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH0, &DMA_initStruct);
	DMA_CH_Open(DMA_CH0);
	
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_BYTE;
	DMA_initStruct.Count = strlen(TX_Buffer);
	DMA_initStruct.SrcAddr = (uint32_t)TX_Buffer;
	DMA_initStruct.SrcAddrInc = 1;
	DMA_initStruct.DstAddr = (uint32_t)&UART0->DATA;
	DMA_initStruct.DstAddrInc = 0;
	DMA_initStruct.Handshake = DMA_CH1_UART0TX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	
	while(1==1)
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < strlen(TX_String[i]); j++)
			{
				UART_WriteByte(UART1, TX_String[i][j]);
				
				while(UART_IsTXBusy(UART1));
			}
			
			for(int j = 0; j < SystemCoreClock/8; j++) __NOP();
		}
	}
}


void UART0_Handler(void)
{
	if(UART_INTStat(UART0, UART_IT_RX_TOUT))
	{
		UART_INTClr(UART0, UART_IT_RX_TOUT);
		
		int str_len = (RX_LEN + RX_LEN - DMA_CH_GetRemaining(DMA_CH0) - RX_Start) % RX_LEN;
		for(int i = 0; i < str_len; i++)
			TX_Buffer[i] = RX_Buffer[(RX_Start + i) % RX_LEN];
		
		RX_Start += str_len;
		
		DMA_CH_SetSrcAddress(DMA_CH1, (uint32_t)TX_Buffer);
		DMA_CH_SetCount(DMA_CH1, str_len);
		DMA_CH_Open(DMA_CH1);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN5, PORTA_PIN5_UART1_RX, 1);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_UART1_TX, 0);
 	
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
 	UART_Init(UART1, &UART_initStruct);
	UART_Open(UART1);
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
	UART_WriteByte(UART1, ch);
	
	while(UART_IsTXBusy(UART1));
 	
	return ch;
}
