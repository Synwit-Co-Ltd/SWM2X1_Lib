#include "SWM2X1.h"

void SerialInit(void);
void PWM0AInit(void);

int main(void)
{
	uint32_t i;
	ADC_InitStructure ADC_initStruct;
	
 	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);
	
#if defined(CHIP_SWM201)
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_CH0, 0);		//PA.14 => ADC0.CH0
	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC0_CH1, 0);		//PA.13 => ADC0.CH1
	PORT_Init(PORTB, PIN3,  PORTB_PIN3_ADC0_CH2,  0);		//PB.3  => ADC0.CH2
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_CH3, 0);		//PA.11 => ADC0.CH3
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ADC0_CH4,  0);		//PB.6  => ADC0.CH4
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ADC0_CH5,  0);		//PB.5  => ADC0.CH5
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ADC0_CH6,  0);		//PB.4  => ADC0.CH6
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_ADC0_CH7,  0);		//PA.8  => ADC0.CH7
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_ADC0_CH8,  0);		//PB.9  => ADC0.CH8
	PORT_Init(PORTB, PIN7,  PORTB_PIN7_ADC0_CH9,  0);		//PB.7  => ADC0.CH9
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_ADC0_CH10, 0);		//PM.9  => ADC0.CH10
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_ADC0_CH11, 0);		//PM.8  => ADC0.CH11
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

#if defined(CHIP_SWM201)
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV8;
	ADC_initStruct.clk_div = 15;
#elif defined(CHIP_SWM211)
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV2;
	ADC_initStruct.clk_div = 6;
#endif
	ADC_initStruct.ref_src = ADC_REFSRC_3V6;
	ADC_initStruct.channels = ADC_CH0;					//PWM ����CH0
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGGER_PWM0;
	ADC_initStruct.Continue = 0;						//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn = ADC_CH0 | ADC_CH1;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//����ADC
	ADC_Open(ADC0);										//ʹ��ADC

	PWM0AInit();
	
#if defined(CHIP_SWM201)
	ADC0->CHSEL &= ~(   0xFF << ADC_CHSEL_SW_Pos);
	ADC0->CHSEL |=  (ADC_CH1 << ADC_CHSEL_SW_Pos);		//�������CH1
	
	while(1==1)
	{
		ADC0->CTRL &= ~ADC_CTRL_TRIG_Msk;		//�������ADCǰ���������г�CPU
		ADC_Start(ADC0);						//���ADC�Ѿ���PWM��������˴��������������
		ADC0->CTRL |= (1 << ADC_CTRL_TRIG_Pos);	//�������ADC�󣬽������г�PWM
		
		for(i = 0; i < SystemCoreClock/1000; i++) __NOP();
	}
#elif defined(CHIP_SWM211)
	while(1==1)
	{
		ADC0->CTRL &= ~ADC_CTRL_TRIG_Msk;		//�������ADCǰ���������г�CPU
		for(i = 0; i < CyclesPerUs; i++) __NOP();		//�ȴ�PWM������ͨ��ת����ɣ���ֹPWM���������ת��ͨ��
		ADC0->CHSEL &= ~(   0xFF << ADC_CHSEL_SW_Pos);
		ADC0->CHSEL |=  (ADC_CH1 << ADC_CHSEL_SW_Pos);	//�������CH1
		ADC0->CTRL = ADC0->CTRL;	//ע�⣺д CTRL �Ĵ������ܸ���ͨ��ѡ��
		ADC_Start(ADC0);
		for(i = 0; i < CyclesPerUs; i++) __NOP();		//�ȴ����������ͨ��ת�����
		
		ADC0->CHSEL &= ~(   0xFF << ADC_CHSEL_SW_Pos);
		ADC0->CHSEL |=  (ADC_CH0 << ADC_CHSEL_SW_Pos);	//���������ͨ����PWM������ͨ��������ȫһ��
		ADC0->CTRL |= (1 << ADC_CTRL_TRIG_Pos);	//�������ADC�󣬽������г�PWM
		
		for(i = 0; i < SystemCoreClock/1000; i++) __NOP();
	}
#endif
}


void ADC0_Handler(void)
{
	GPIO_InvBit(GPIOA, PIN5);
	
	if(ADC_IntEOCStat(ADC0, ADC_CH0))
	{
		ADC_IntEOCClr(ADC0, ADC_CH0);
		
		printf("%d,", ADC_Read(ADC0, ADC_CH0));
	}
	else if(ADC_IntEOCStat(ADC0, ADC_CH1))
	{
		ADC_IntEOCClr(ADC0, ADC_CH1);
		
// 		printf("%d,", ADC_Read(ADC0, ADC_CH1));
	}
}


void PWM0AInit(void)
{
	PWM_InitStructure PWM_initStruct;
	
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PWM_initStruct.Mode = PWM_CENTER_ALIGNED;
	PWM_initStruct.Clkdiv = 6;					//F_PWM = 30M/6 = 5M
	PWM_initStruct.Period = 50000;				//5M/50000 = 100Hz�����ĶԳ�ģʽ��Ƶ�ʽ��͵�50Hz
	PWM_initStruct.HdutyA = 12500;				//12500/50000 = 25%
	PWM_initStruct.DeadzoneA = 50;				//50/5M = 10us
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB = 25000;				//25000/50000 = 50%
	PWM_initStruct.DeadzoneB = 50;				//50/5M = 10us
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
	
	
	/* Mask */
	PWM_CmpTrigger(PWM0, 1500, PWM_DIR_UP, 64, PWM_TRG_1, 0);	//PWM0���ϼ�������ֵ����1500ʱ����һ�������źţ������źŷ��͵� trigger1
	
// 	PWM_OutMask(PWM0, PWM_CH_A, PWM_EVT_1, 0, PWM_EVT_1, 1);	//PWM0A��PWM0AN��event1Ϊ��ʱ�ֱ����0��1
// 	PWM_OutMask(PWM0, PWM_CH_B, PWM_EVT_1, 0, PWM_EVT_1, 1);	//PWM0B��PWM0BN��event1Ϊ��ʱ�ֱ����0��1
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
