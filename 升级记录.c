2022/06/17
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_uart.c 文件中，UART_SetBaudrate() 函数中，将：
UARTx->BAUD &= ~UART_BAUD_BAUD_Msk;
修正为：
UARTx->BAUD &= ~(UART_BAUD_BAUD_Msk | UART_BAUD_FRAC_Msk);

2022/07/25
CSL\SWM2X1_StdPeriph_Driver\SWM211_port.h 文件中，将：
#define PORTM_PIN8_PWM0BN       2
#define PORTM_PIN8_TIMR0_IN     3
#define PORTM_PIN8_TIMR0_OUT    4
修正为：
#define PORTM_PIN8_CAN0_RX		2
#define PORTM_PIN8_PWM0BN       3
#define PORTM_PIN8_TIMR0_IN     4
#define PORTM_PIN8_TIMR0_OUT    5

2022/07/27
CSL\CMSIS\DeviceSupport\system_SWM211.c 文件中，SystemInit() 函数中，删除：
	PORTA->PULLD = 0xFFFF;	// 下拉电阻：0 开启   1 关闭
	PORTB->PULLD = 0xFFFF;
	PORTM->PULLD = 0xFFFF;

CSL\CMSIS\DeviceSupport\system_SWM211.c 文件中，SystemInit() 函数中，将：
//	PORTB->PULLD &= ~(1 << PIN11);
修改为：
	PORTB->PULLD &= ~((1 << PIN10) | (1 << PIN11));

CSL\CMSIS\DeviceSupport\system_SWM211.c 文件中，switchToXTAL() 函数中，将：
	PORTB->PULLD |=  ((1 << PIN11) | (1 << PIN12));		// 下拉电阻：0 开启   1 关闭
修改为：
	PORTB->PULLD &= ~((1 << PIN11) | (1 << PIN12));

CSL\CMSIS\DeviceSupport\system_SWM211.c 文件中，PLLInit() 函数中，将：
	PORTB->PULLD |=  ((1 << PIN11) | (1 << PIN12));		// 下拉电阻：0 开启   1 关闭
修改为：
	PORTB->PULLD &= ~((1 << PIN11) | (1 << PIN12));

CSL\SWM2X1_StdPeriph_Driver\SWM2X1_gpio.c 文件中，将：
#if defined(CHIP_SWM211)
	if(pull_down == 1)   PORTx->PULLD &= ~(1 << n);		// 下拉电阻：0 开启   1 关闭
	else                 PORTx->PULLD |=  (1 << n);
#else
	if(pull_down == 1)   PORTx->PULLD |=  (1 << n);
	else                 PORTx->PULLD &= ~(1 << n);
#endif
修改为：
	if(pull_down == 1)   PORTx->PULLD |=  (1 << n);
	else                 PORTx->PULLD &= ~(1 << n);

CSL\SWM2X1_StdPeriph_Driver\SWM2X1_adc.c 文件中，ADC_Init() 函数中，将：
		ADCx->CTRL4 &= ~ADC_CTRL4_EREFSEL_Msk;
		ADCx->CTRL4 |= ((initStruct->ref_src & 0xF) << ADC_CTRL4_EREFSEL_Pos);
修改为：
#if defined(CHIP_SWM201)
		ADCx->CTRL4 &= ~ADC_CTRL4_EREFSEL_Msk;
		ADCx->CTRL4 |= ((initStruct->ref_src & 0xF) << ADC_CTRL4_EREFSEL_Pos);
#else
		ADCx->CTRL2 &= ~(1 << 28);
		ADCx->CTRL2 |= ((initStruct->ref_src & 0xF) << 28);
#endif

2022/09/22
CSL\CMSIS\DeviceSupport\system_SWM211.c 文件中，SystemInit() 函数中，添加：
	SYS->PGACR &= ~SYS_PGACR_VRTRIM_Msk;	//PGA正输入端参考电压校准
	SYS->PGACR |= ((SYS->CHIPID[3] & 0x000F) << SYS_PGACR_VRTRIM_Pos);

	SYS->PGACR &= ~SYS_PGACR_DATRIM_Msk;	//用于产生ACMP N输入端电压的DAC的参考电压校准
	SYS->PGACR |= (((SYS->CHIPID[3] >> 16) & 0x000F) << SYS_PGACR_DATRIM_Pos);

2022/10/20
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_uart.c 文件中，将：
void UART_INTRXThresholdEn(UART_TypeDef * UARTx);
void UART_INTRXThresholdDis(UART_TypeDef * UARTx);
uint32_t UART_INTRXThresholdStat(UART_TypeDef * UARTx);
void UART_INTTXThresholdEn(UART_TypeDef * UARTx);
void UART_INTTXThresholdDis(UART_TypeDef * UARTx);
uint32_t UART_INTTXThresholdStat(UART_TypeDef * UARTx);
void UART_INTTimeoutEn(UART_TypeDef * UARTx);
void UART_INTTimeoutDis(UART_TypeDef * UARTx);
uint32_t UART_INTTimeoutStat(UART_TypeDef * UARTx);
void UART_INTTXDoneEn(UART_TypeDef * UARTx);
void UART_INTTXDoneDis(UART_TypeDef * UARTx);
uint32_t UART_INTTXDoneStat(UART_TypeDef * UARTx);
简化为：
void UART_INTEn(UART_TypeDef * UARTx, uint32_t it);
void UART_INTDis(UART_TypeDef * UARTx, uint32_t it);
void UART_INTClr(UART_TypeDef * UARTx, uint32_t it);
uint32_t UART_INTStat(UART_TypeDef * UARTx, uint32_t it);

CSL\SWM2X1_StdPeriph_Driver\SWM2X1_uart.c 文件中，UART_Init() 函数中，将：
	UARTx->TOCR |= (initStruct->TimeoutTime << UART_TOCR_TIME_Pos);
修改为：
	UARTx->TOCR |= (1 << UART_TOCR_MODE_Pos) |
				   (initStruct->TimeoutTime << UART_TOCR_TIME_Pos);

修改 CSL\SWM2X1_StdPeriph_Driver\SWM2X1_dma.c 和 SWM2X1_dma.h 文件，使用 Scatter-Gather 模式实现 ping-pong 功能

2022/10/21
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_can.h 文件中，将：
#define CAN_INT_RX_NOTEMPTY	(0x01 << 0)		//RX Buffer Not Empty
#define CAN_INT_RX_OVERFLOW	(0x01 << 3)		//RX Buffer Overflow
#define CAN_INT_TX_EMPTY	(0x01 << 1)		//TX Buffer Empty
#define CAN_INT_ARBLOST		(0x01 << 6)		//Arbitration lost
#define CAN_INT_ERR			(0x01 << 7)
#define CAN_INT_ERR_WARN	(0x01 << 2)		//TXERR/RXERR计数值达到Error Warning Limit
#define CAN_INT_ERR_PASS	(0x01 << 5)		//TXERR/RXERR计数值达到127
#define CAN_INT_WAKEUP		(0x01 << 4)
修改为：
#define CAN_IT_RX_NOTEMPTY	(0x01 << 0)		//RX Buffer Not Empty
#define CAN_IT_RX_OVERFLOW	(0x01 << 3)		//RX Buffer Overflow
#define CAN_IT_TX_EMPTY		(0x01 << 1)		//TX Buffer Empty
#define CAN_IT_ARBLOST		(0x01 << 6)		//Arbitration lost
#define CAN_IT_ERR			(0x01 << 7)
#define CAN_IT_ERR_WARN		(0x01 << 2)		//TXERR/RXERR计数值达到Error Warning Limit
#define CAN_IT_ERR_PASS		(0x01 << 5)		//TXERR/RXERR计数值达到127
#define CAN_IT_WAKEUP		(0x01 << 4)

2022/10/31
CSL\SWM2X1_StdPeriph_Driver\SWM211_port.h 文件中，删除：
#define PORTA_PIN13_ADC0_CH4    7
ADC0_CH4 是一个单独的 PAD，在不同的封装上连接到不同的引脚

2022/11/03
更正 iar 和 gcc 的 startup_SWM201.s 和 startup_SWM211.s 文件

2022/11/08
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_adc.c 文件中，ADC_Init() 函数中，添加：
#elif defined(CHIP_SWM211)
		ADCx->CALIBSET = SYS->BACKUP[1];
		ADCx->CALIBEN = (1 << ADC_CALIBEN_K_Pos) | (1 << ADC_CALIBEN_OFFSET_Pos);
开启 SWM211 ADC 5V 基准测量时的校准功能

2022/11/10
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_adc.h 文件中，修改 ADC_CLKSRC_xx 宏定义，实现更灵活的 ADC 时钟分频
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_adc.c 文件中，ADC_Init() 函数中将：
	ADCx->CTRL3 |= ((initStruct->clk_src >> 4) << ADC_CTRL3_CLKDIV1_Pos) |
修改为：
	ADCx->CTRL3 |= ((initStruct->clk_src >> 6) << ADC_CTRL3_CLKDIV1_Pos) |

	ADCx->CTRL4 |= (2 << ADC_CTRL4_CLKDIV0_Pos);
修改为：
	ADCx->CTRL4 |= (((initStruct->clk_src >> 4) & 3) << ADC_CTRL4_CLKDIV0_Pos);
以匹配对 ADC_CLKSRC_xx 宏定义的修改

2022/11/17
CSL\SWM2X1_StdPeriph_Driver\SWM2X1_timr.c 文件中，TIMR_Init() 函数中，将：
	{
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_TIMR_Pos);
		
		TIMR_Stop(TIMRx);	//一些关键寄存器只能在定时器停止时设置
		
		TIMRx->CR &= ~(TIMR_CR_MODE_Msk | TIMR_CR_CLKSRC_Msk);
		TIMRx->CR |= (mode << TIMR_CR_CLKSRC_Pos);
	}
	else if((TIMRx == BTIMR0) || (TIMRx == BTIMR1) || (TIMRx == BTIMR2) || (TIMRx == BTIMR3))
	{
		SYS->CLKEN1 |= (0x01 << SYS_CLKEN1_BTIMR_Pos);
		
		TIMR_Stop(TIMRx);	//一些关键寄存器只能在定时器停止时设置
		
		if(mode == TIMR_MODE_OC) TIMRx->CR = 1;
	}
修正为：
	{
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_TIMR_Pos);
	}
	else if((TIMRx == BTIMR0) || (TIMRx == BTIMR1) || (TIMRx == BTIMR2) || (TIMRx == BTIMR3))
	{
		SYS->CLKEN1 |= (0x01 << SYS_CLKEN1_BTIMR_Pos);
	}
	
	TIMR_Stop(TIMRx);	//一些关键寄存器只能在定时器停止时设置
		
	TIMRx->CR &= ~(TIMR_CR_MODE_Msk | TIMR_CR_CLKSRC_Msk);
	TIMRx->CR |= (mode << TIMR_CR_CLKSRC_Pos);
解决 SWM211 中 BTIMRx 配置 TIMR_MODE_OC 模式不起作用的问题

CSL\SWM2X1_StdPeriph_Driver\SWM2X1_timr.c 文件中，TIMR_OC_Init() 函数中，将：
#if defined(CHIP_SWM211)
	case ((uint32_t)TIMR2):
		if(match_int_en) NVIC_EnableIRQ(GPIOB0_GPIOA8_TIMR2_IRQn);
		break;
#endif
修正为：
#if defined(CHIP_SWM211)
	case ((uint32_t)TIMR2):
		if(match_int_en) NVIC_EnableIRQ(GPIOB0_GPIOA8_TIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR0):
		if(match_int_en) NVIC_EnableIRQ(BTIMR0_IRQn);
		break;
	
	case ((uint32_t)BTIMR1):
		if(match_int_en) NVIC_EnableIRQ(BTIMR1_IRQn);
		break;
	
	case ((uint32_t)BTIMR2):
		if(match_int_en) NVIC_EnableIRQ(BTIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR3):
		if(match_int_en) NVIC_EnableIRQ(BTIMR3_IRQn);
		break;
#endif
