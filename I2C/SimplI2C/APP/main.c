#include "SWM2X1.h"
#include "IOI2C_SLV.h"


/* ע�⣺
 *	1����������������ֵ�ϴ����������������ޣ��� MstClk > 100000 ʱ��
 *	   ������������裨500KHz ʱ 3K����1MHz ʱ 1K���������� I2C ���Ƶ�ʻ�����趨ֵ
 */


#define SLV_ADDR  0x6C

char mst_txbuff[4] = {0x37, 0x55, 0xAA, 0x78};
char mst_rxbuff[4] = {0};

void SerialInit(void);
void I2C_Mst_Init(void);

int main(void)
{
	uint32_t i;
	uint8_t ack;
	
	SystemInit();
	
	SerialInit();
	
	IOI2C_SLV_Init();
	
	I2C_Mst_Init();
	
	while(1==1)
	{
		/*************************** Master Write ************************************/		
		ack = I2C_Start(I2C0, (SLV_ADDR << 1) | 0, 1);
		if(ack == 0)
		{
			printf("Slave send NACK for address\r\n");
			goto nextloop;
		}
		
		for(i = 0; i < 4; i++)
		{
			ack = I2C_Write(I2C0, mst_txbuff[i], 1);
			if(ack == 0)
			{
				printf("Slave send NACK for data\r\n");
				goto nextloop;
			}
		}
		
		I2C_Stop(I2C0, 1);
		
		printf("Master Send %X %X %X %X\r\n", mst_txbuff[0], mst_txbuff[1], mst_txbuff[2], mst_txbuff[3]);
		
		
		/********************************** Master Read *******************************/		
		ack = I2C_Start(I2C0, (SLV_ADDR << 1) | 1, 1);
		if(ack == 0)
		{
			printf("Slave send NACK for address\r\n");
			goto nextloop;
		}
		
		for(i = 0; i < 3; i++)
		{
			mst_rxbuff[i] = I2C_Read(I2C0, 1, 1);
		}
		mst_rxbuff[i] = I2C_Read(I2C0, 0, 1);
		
		printf("Master Read %X %X %X %X\r\n", mst_rxbuff[0], mst_rxbuff[1], mst_rxbuff[2], mst_rxbuff[3]);
		
		if((mst_txbuff[0] == mst_rxbuff[0]) && (mst_txbuff[1] == mst_rxbuff[1]) && (mst_txbuff[2] == mst_rxbuff[2]) && (mst_txbuff[3] == mst_rxbuff[3]))
			printf("Success\r\n");
		else
			printf("Fail\r\n");

nextloop:
		I2C_Stop(I2C0, 1);
		for(i = 0; i < SystemCoreClock/4; i++) ;
	}
}


void I2C_Mst_Init(void)
{
	I2C_InitStructure I2C_initStruct;
	
	PORT_Init(PORTA, PIN6, PORTA_PIN6_I2C0_SCL, 1);		//GPIOA.6����ΪI2C0 SCL����
	PORTA->OPEND |= (1 << PIN6);						//��©���
	PORTA->PULLU |= (1 << PIN6);						//ʹ������
	PORT_Init(PORTA, PIN7, PORTA_PIN7_I2C0_SDA, 1);		//GPIOA.7����ΪI2C0 SDA����
	PORTA->OPEND |= (1 << PIN7);						//��©���
	PORTA->PULLU |= (1 << PIN7);						//ʹ������
	
	I2C_initStruct.Master = 1;
	I2C_initStruct.MstClk = 100000;
	I2C_initStruct.Addr10b = 0;
	I2C_initStruct.TXEmptyIEn = 0;
	I2C_initStruct.RXNotEmptyIEn = 0;
	I2C_Init(I2C0, &I2C_initStruct);
	
	I2C_Open(I2C0);
}


uint32_t IOI2C_SLV_EmptyCallback(char *TxBuff)
{
	uint32_t i = 0;
	
	while(IOI2C_SLV_RxNotEmpty())
		TxBuff[i++] = IOI2C_SLV_FetchByte();
	
	return i;
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
