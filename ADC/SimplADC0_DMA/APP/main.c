#include <string.h>
#include "SWM2X1.h"

#define ADC_SIZE  1250
uint16_t ADC_Result[ADC_SIZE] = {0};


void SerialInit(void);

int main(void)
{
	ADC_InitStructure ADC_initStruct;
	DMA_InitStructure DMA_initStruct;
	
	SystemInit();
	
	SerialInit();
	
#if defined(CHIP_SWM201)
	while(1)
	{
	}
#elif defined(CHIP_SWM211)
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_CH0, 0);		//PA.14 => ADC0.CH0
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_CH1, 0);		//PA.11 => ADC0.CH1
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_ADC0_CH2,  0);		//PA.8  => ADC0.CH2
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_ADC0_CH3,  0);		//PB.9  => ADC0.CH3
	PORT_Init(PORTB, PIN7,  PORTB_PIN7_ADC0_CH5,  0);		//PB.7  => ADC0.CH5
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ADC0_CH6,  0);		//PB.6  => ADC0.CH6
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ADC0_CH7,  0);		//PB.5  => ADC0.CH7
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ADC0_CH8,  0);		//PB.4  => ADC0.CH8
	PORT_Init(PORTB, PIN3,  PORTB_PIN3_ADC0_CH9,  0);		//PB.3  => ADC0.CH9
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_ADC0_CH10, 0);		//PM.9  => ADC0.CH10
	
	/* ע�⣺
	 * �� SWM211C8 �ϣ�ADC0_CH4 ��ͨ����ʼ�����Ϊ�� PORT_Init(PORTB, PIN0, 0, 0);
	 *
	 * �� SWM21PG6 �ϣ�ADC0_CH4 ��ͨ����ʼ�����Ϊ�� PORT_Init(PORTM, PIN4, 0, 0);
	 *
	 * �� SWM21DK6 �ϣ�ADC0_CH4 ��ͨ����ʼ�����Ϊ�� PORT_Init(PORTM, PIN8, 0, 0);
	 *
	 ****************************************************************************/
#endif

	/* ADC����ʱ��Ϊ 12MHz / 2 / 6 = 1MHz
	   ��һ��ת��ͨ�����ת����Ҫ����ʱ�����ڣ�һ������ת�����������ڵȴ�������˵�ͨ��ת��ʱ�����ת������Ϊ 1MHz / 3 = 333KSPS
	   ������ת��ͨ�����ת����Ҫһ��ʱ�����ڣ�һ������ת����û�еȴ����ڣ������˫ͨ��ת��ʱ�����ת������Ϊ 1MHz / (3 + 1) * 2 = 500KSPS
	   ����Ҫע����ǣ�΢����ÿ��ͨ����ת������һ����������ɵģ���΢��ת������һֱ�� 1MSPS
	*/
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV2;
	ADC_initStruct.clk_div = 6;
	ADC_initStruct.ref_src = ADC_REFSRC_3V6;
	ADC_initStruct.channels = ADC_CH0;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_initStruct.Continue = 1;						//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//����ADC
	ADC_Open(ADC0);										//ʹ��ADC
	
	ADC0->CTRL |= (1 << ADC_CTRL_RES2FIFO_Pos) | (1 << ADC_CTRL_DMAEN_Pos);

	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_HALFWORD;
	DMA_initStruct.Count = ADC_SIZE;
	DMA_initStruct.SrcAddr = (uint32_t)&ADC0->FIFODR;
	DMA_initStruct.SrcAddrInc = 0;
	DMA_initStruct.DstAddr = (uint32_t)ADC_Result;
	DMA_initStruct.DstAddrInc = 1;
	DMA_initStruct.Handshake = DMA_CH1_ADC0;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = DMA_IT_DONE;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	DMA_CH_Open(DMA_CH1);

	ADC_Start(ADC0);

	while(1==1)
	{
	}
}


void GPIOB1_GPIOA9_DMA_Handler(void)
{
	uint32_t i, chn;
	
	if(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE))
	{
		DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
		
		ADC_Stop(ADC0);
		
		for(i = 0; i < ADC_SIZE; i++)
		{
			chn = (ADC_Result[i] & ADC_DATA_NUM_Msk) >> ADC_DATA_NUM_Pos;
			switch(chn)
			{
			case 0:	// ͨ��0
				printf("%4d,", ADC_Result[i] & ADC_DATA_VAL_Msk);
				break;
			}
		}
		
		DMA_CH_Open(DMA_CH1);	// ���¿�ʼ���ٴΰ���
		
		ADC_Start(ADC0);
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
