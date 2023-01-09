#include "SWM2X1.h"

void SerialInit(void);

int main(void)
{	
	uint32_t i;
	uint32_t rxdata, txdata = 0x23;
	SPI_InitStructure SPI_initStruct;
	
	SystemInit();
	
	SerialInit();
	
#ifdef CHIP_SWM201
#if 1
	PORT_Init(PORTB, PIN11, PORTB_PIN11_SPI0_SCLK, 0);
	PORT_Init(PORTB, PIN12, PORTB_PIN12_SPI0_MOSI, 0);	//��MOSI��MISO���ӣ��Է������ա�Ȼ���ӡ
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI0_MISO, 1);
	PORT_Init(PORTB, PIN15, PORTB_PIN15_SPI0_SSEL, 0);
#else
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_SPI0_SCLK,  0);
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_SPI0_MOSI,  0);
	PORT_Init(PORTM, PIN11, PORTM_PIN11_SPI0_MISO, 1);
	PORT_Init(PORTM, PIN10, PORTM_PIN10_SPI0_SSEL, 0);
#endif
#elif defined(CHIP_SWM211)
	PORT_Init(PORTM, PIN7,  PORTM_PIN7_SPI0_SSEL,  0);
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_SPI0_SCLK,  0);
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_SPI0_MOSI,  0);	//��MOSI��MISO���ӣ��Է������ա�Ȼ���ӡ
	PORT_Init(PORTM, PIN10, PORTM_PIN10_SPI0_MISO, 1);
	
	PORT_Init(PORTB, PIN10, PORTB_PIN10_SPI1_SCLK, 0);
	PORT_Init(PORTB, PIN13, PORTB_PIN13_SPI1_MOSI, 0);
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI1_MISO, 1);
	PORT_Init(PORTB, PIN15, PORTB_PIN15_SPI1_SSEL, 0);
#endif

	SPI_initStruct.clkDiv = SPI_CLKDIV_4;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 0;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	SPI_Open(SPI0);
	
	while(1==1)
	{
		rxdata = SPI_ReadWrite(SPI0, txdata);
		txdata = rxdata + 1;		//��SPI0_MOSI��SPI0_MISO�̽ӣ���ɿ�����ӡ��ֵ��һ����
		
		printf("rxdata: 0x%X\r\n", rxdata);
		
		for(i = 0; i < SystemCoreClock/10; i++);
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
