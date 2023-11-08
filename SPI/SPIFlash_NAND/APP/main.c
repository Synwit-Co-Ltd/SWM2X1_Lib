#include <string.h>
#include "SWM2X1.h"
#include "W25N01G.h"


#define EEPROM_ADDR	  0x0020000

#define N_DATA  48

uint8_t RdBuff[W25N_PAGE_SIZE] = {0};
uint8_t WrBuff[W25N_PAGE_SIZE] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};


void SerialInit(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
	
	W25N01G_Init();
	
	int id = W25N01G_ReadJEDEC();
	printf("\n\nSPI Flash JEDEC: %06X\n", id);
	
	W25N01G_FlashProtect(W25N_PROTECT_Upper_1MB);
	
	W25N01G_Erase(EEPROM_ADDR, 1);
	
	W25N01G_Read(EEPROM_ADDR, RdBuff);
	
	printf("\n\nAfter Erase: \n");
	for(int i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	W25N01G_Write(EEPROM_ADDR, WrBuff);
	
	W25N01G_Read(EEPROM_ADDR, RdBuff);
	
	printf("\n\nAfter Write: \n");
	for(int i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
#ifdef CHIP_SWM211
	memset(RdBuff, 0x00, N_DATA);
	W25N01G_Read_4bit_DMA(EEPROM_ADDR, RdBuff);
	
	printf("\n\nQuad Read: \n");
	for(int i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
#endif

	while(1==1)
	{
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
