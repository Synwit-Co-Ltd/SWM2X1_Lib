#include "SWM2X1.h"


/* ����������SWM211 <==CAN_RX/TX==> CAN �շ������� TJA1050��<==CAN_H/L==> CAN ������
 * ��������ʾ��
 *	1���� CAN �շ����� CAN �����ǶϿ������� ACK ����CAN->TXERR �� 0 ������ 128��Ȼ�󱣳ֲ���
 *	2���� SWM211 �� CAN �շ����Ͽ�������λ����CAN->TXERR ���������� 255������ Bus Off��CAN->CR.RST �Զ��� 1��CAN ģ�鴦�ڸ�λ״̬
 *	3��ISR ���ڼ�⵽ Bus Off ��ִ�� CAN_Open(CAN0) ʹ CAN ģ���˳���λ״̬����ʱ������Ͽ������ӻָ���CAN->TXERR ���𽥵ݼ���0��CAN ģ��ָ���������
*/


void SerialInit(void);

int main(void)
{
	CAN_InitStructure CAN_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTM, PIN3, PORTM_PIN3_CAN0_RX, 1);	//GPIOM.3����ΪCAN0��������
	PORT_Init(PORTM, PIN4, PORTM_PIN4_CAN0_TX, 0);	//GPIOM.4����ΪCAN0�������
	
	CAN_initStruct.Mode = CAN_MODE_NORMAL;
	CAN_initStruct.CAN_bs1 = CAN_BS1_5tq;
	CAN_initStruct.CAN_bs2 = CAN_BS2_4tq;
	CAN_initStruct.CAN_sjw = CAN_SJW_2tq;
	CAN_initStruct.Baudrate = 50000;
	CAN_initStruct.RXNotEmptyIEn = 1;
	CAN_initStruct.ArbitrLostIEn = 0;
	CAN_initStruct.ErrPassiveIEn = 1;
	CAN_Init(CAN0, &CAN_initStruct);
	
	CAN_INTEn(CAN0, CAN_IT_ERR_WARN);
	
	CAN_SetFilter32b(CAN0, CAN_FILTER_1, 0x00122122, 0x1FFFFFFE);		//����IDΪ0x00122122��0x00122123����չ��
	CAN_SetFilter16b(CAN0, CAN_FILTER_2, 0x122, 0x7FE, 0x101, 0x7FF);	//����IDΪ0x122��123��0x101�ı�׼��
	
	CAN_Open(CAN0);
	
	while(1)
	{
		uint32_t i;
		uint8_t tx_data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	
		CAN_Transmit(CAN0, CAN_FRAME_STD, 0x133, tx_data, 8, 1);
		while(CAN_TXComplete(CAN0) == 0) __NOP();
		
		printf("\r\nCAN->TXERR: %d\r\n", CAN0->TXERR);
		
		for(i = 0; i < SystemCoreClock / 8; i++) __NOP();
	}
}


void GPIOB2_GPIOA10_CAN0_Handler(void)
{
	uint32_t can_if = CAN_INTStat(CAN0);
	
	if(can_if & CAN_IF_RXDA_Msk)
	{
		uint32_t i;
		CAN_RXMessage msg;
		
		CAN_Receive(CAN0, &msg);
		
		if(msg.size > 0)
		{
			printf("\r\nReceive %s: %08X, ", msg.format == CAN_FRAME_STD ? "STD" : "EXT", msg.id);
			for(i = 0; i < msg.size; i++) printf("%02X, ", msg.data[i]);
			printf("\r\n");
		}
		else if(msg.remote == 1)	//Զ��֡
		{
			printf("\r\nReceive %s Remote Request\r\n", msg.format == CAN_FRAME_STD ? "STD" : "EXT");
		}
	}
	
	if(can_if & CAN_IF_ERRWARN_Msk)
	{
		if(CAN0->SR & CAN_SR_BUSOFF_Msk)
		{
			printf("\r\nCAN Bus Off\r\n");
			printf("\r\nCAN->CR.RST = %d\r\n", CAN0->CR & CAN_CR_RST_Msk ? 1 : 0);
			
			CAN_Open(CAN0);			//Bus Off recovery
		}
		else if(CAN0->SR & CAN_SR_ERRWARN_Msk)
		{
			printf("\r\nCAN Error Warning\r\n");
		}
	}
	
	if(can_if & CAN_IF_ERRPASS_Msk)
	{
		printf("\r\nCAN Error Passive\r\n");
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
