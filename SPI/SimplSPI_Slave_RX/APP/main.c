#include "SWM2X1.h"
#include "IOSPI.h"
#include <string.h>

#define RX_COUNT  32
uint16_t SPI_RXBuffer[RX_COUNT] = {0};
uint32_t SPI_RXIndex = 0;
volatile uint32_t SPI_RXFinish = 0;

void SerialInit(void);
void SPISlvInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();	
	
	SPISlvInit();
	
	IOSPI_Init();
	
	while(1==1)
	{
		IOSPI_CS_Low();
		
		for(i = 0; i < 16; i++)
			IOSPI_ReadWrite(i | (i << 4), 8);
		
		IOSPI_CS_High();
		
		while(SPI_RXFinish == 0) __NOP();
		SPI_RXFinish = 0;
		
		for(i = 0; i < SPI_RXIndex; i++)
			printf("%02X, ", SPI_RXBuffer[i]);
		printf("\r\n\r\n");
		
		SPI_RXIndex = 0;
		memset(SPI_RXBuffer, 0, 64);
		
		for(i = 0; i < SystemCoreClock/10; i++) __NOP();
	}
}


void SPISlvInit(void)
{
	SPI_InitStructure SPI_initStruct;
	
#ifdef CHIP_SWM201
	PORT_Init(PORTB, PIN11, PORTB_PIN11_SPI0_SCLK, 1);
	PORT_Init(PORTB, PIN12, PORTB_PIN12_SPI0_MOSI, 1);
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI0_MISO, 0);
	PORT_Init(PORTB, PIN15, PORTB_PIN15_SPI0_SSEL, 1);
#elif defined(CHIP_SWM211)
	PORT_Init(PORTM, PIN7,  PORTM_PIN7_SPI0_SSEL,  1);
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_SPI0_SCLK,  1);
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_SPI0_MOSI,  1);
	PORT_Init(PORTM, PIN10, PORTM_PIN10_SPI0_MISO, 0);
#endif
	
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 0;
	SPI_initStruct.RXThreshold = 4;		//����FIFO�����ݸ�����CTRL.RFTHRʱ�����ж�
	SPI_initStruct.RXThresholdIEn = 1;
	SPI_initStruct.TXThreshold = 0;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	
#ifdef CHIP_SWM201
	/* ��SPI0_SSEL�������������ڼ��SPI�ӻ���CS Deassert�¼� */
	GPIO_Init(GPIOA, PIN9, 0, 1, 0, 0);			//���룬����ʹ��
	
	EXTI_Init(GPIOA, PIN9, EXTI_RISE_EDGE);		//�����ش����ж�
	
	NVIC_EnableIRQ(GPIOB1_GPIOA9_IRQn);

	EXTI_Open(GPIOA, PIN9);
#else
	SPI_INTEn(SPI0, SPI_IT_CS_RISE);
#endif

	SPI_Open(SPI0);
}

void GPIOB3_GPIOA11_SPI0_Handler(void)
{
	if(SPI0->IF & SPI_IF_RFTHR_Msk)
	{
		while(SPI0->STAT & SPI_STAT_RFNE_Msk)
		{
			SPI_RXBuffer[SPI_RXIndex] = SPI0->DATA;
			if(SPI_RXIndex < RX_COUNT - 1) SPI_RXIndex++;
		}
		
		SPI0->IF = (1 << SPI_IF_RFTHR_Pos);	// �ȶ������ݣ��������־
	}
#ifdef CHIP_SWM211
	else if(SPI0->IF & SPI_IF_CSRISE_Msk)
	{
		SPI0->IF = SPI_IF_CSRISE_Msk;
		
		while(SPI0->STAT & SPI_STAT_RFNE_Msk)
		{
			SPI_RXBuffer[SPI_RXIndex] = SPI0->DATA;
			if(SPI_RXIndex < RX_COUNT - 1) SPI_RXIndex++;
		}
		
		SPI_RXFinish = 1;
	}
#endif
}


#ifdef CHIP_SWM201
void GPIOB1_GPIOA9_Handler(void)
{	
	EXTI_Clear(GPIOA, PIN9);
	
	while(SPI0->STAT & SPI_STAT_RFNE_Msk)
	{
		SPI_RXBuffer[SPI_RXIndex] = SPI0->DATA;
		if(SPI_RXIndex < RX_COUNT - 1) SPI_RXIndex++;
	}
	
	SPI_RXFinish = 1;
}
#endif


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
