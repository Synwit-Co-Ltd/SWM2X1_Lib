#include "SWM2X1.h"
#include "W25N01G.h"


/****************************************************************************************************************************************** 
* ��������:	W25N01G_Init()
* ����˵��:	W25N01G ��ʼ��
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void W25N01G_Init(void)
{
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOB, PIN15, 1, 0, 0, 0);
#define W25N01G_Assert()	GPIO_ClrBit(GPIOB, PIN15)
#define W25N01G_Deassert()	GPIO_SetBit(GPIOB, PIN15)
	W25N01G_Deassert();
	
#ifdef CHIP_SWM201
	PORT_Init(PORTB, PIN11, PORTB_PIN11_SPI0_SCLK, 0);
	PORT_Init(PORTB, PIN12, PORTB_PIN12_SPI0_MOSI, 0);
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI0_MISO, 1);
#elif defined(CHIP_SWM211)
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_SPI0_SCLK,  0);
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_SPI0_MOSI,  1);	// ע�⣺MOSI������ʹ�ܱ��뿪������ΪQSPIģʽ����Ҫʹ�ô��߶�ȡ
	PORT_Init(PORTM, PIN10, PORTM_PIN10_SPI0_MISO, 1);
	
	/* for QSPI */
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_SPI0_DATA2, 1);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_SPI0_DATA3, 1);
	PORTB->PULLU |= (1 << PIN1);						// ע�⣺/HOLD ����Ϊ�͵�ƽ�ᵼ��оƬ��ȫû�з�Ӧ
#endif

	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_FIRST_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 0;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	SPI_Open(SPI0);
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG2);
	
	reg &= ~(1 << W25N_STATUS_REG2_BUF_Pos);	// Continuous Read Mode
	
	W25N01G_WriteReg(W25N_STATUS_REG2, reg);
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_ReadJEDEC()
* ����˵��:	W25N01G ��ȡ JEDEC ID
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
uint32_t W25N01G_ReadJEDEC(void)
{
	uint8_t manufacture_id;
	uint16_t device_id;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_READ_JEDEC);
	SPI_ReadWrite(SPI0, 0x00);
	
	manufacture_id = SPI_ReadWrite(SPI0, 0xFF);
	device_id      = SPI_ReadWrite(SPI0, 0xFF) << 8;
	device_id     |= SPI_ReadWrite(SPI0, 0xFF);
	
	W25N01G_Deassert();
	
	return (manufacture_id << 16) | device_id;
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_Erase()
* ����˵��:	W25N01G ����������СΪ 128KB
* ��    ��: uint32_t addr			Ҫ������ SPI Flash ��ַ������ 128KB ���루�� addr �� 0x20000 ����������
*			uint8_t wait			�Ƿ�ȴ� SPI Flash ��ɲ���������1 �ȴ����   0 ��������
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void W25N01G_Erase(uint32_t addr, uint8_t wait)
{
	uint32_t page_addr = addr >> 12;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_ENABLE);
	
	W25N01G_Deassert();
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_ERASE_BLOCK128KB);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	if(wait)
		while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_Write()
* ����˵��:	W25N01G ҳд�룬ҳ��СΪ 2112 Byte
* ��    ��: uint32_t addr			Ҫд�뵽�� SPI Flash ��ַ�������� 0x1000���� 4096����������
*			uint8_t buff[2048]		Ҫд�� SPI Flash �����ݣ������С������ 2048 �ֽڣ�ECC ����������Ӳ����������
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void W25N01G_Write(uint32_t addr, uint8_t buff[2048])
{
	uint32_t page_addr = addr >> 12;
	uint16_t column_addr = 0x00;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_ENABLE);
	
	W25N01G_Deassert();
	
	/* Load the program data into the Data Buffer */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PAGE_PROGRAM);
	SPI_ReadWrite(SPI0, (column_addr >> 8) & 0xFF);
	SPI_ReadWrite(SPI0, (column_addr >> 0) & 0xFF);
	
	for(int i = 0; i < 2048; i++)
	{
		SPI_ReadWrite(SPI0, buff[i]);
	}
	
	W25N01G_Deassert();
	
	/* Program the Data Buffer content into the physical memory page */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PROGRAM_EXECUTE);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_Read()
* ����˵��:	W25N01G ҳ��ȡ��ҳ��СΪ 2112 Byte
* ��    ��: uint32_t addr			Ҫ��ȡ�Ե� SPI Flash ��ַ�������� 0x1000���� 4096����������
*			uint8_t buff[2048]		��ȡ��������д��������У������С������ 2048 �ֽڣ�ECC ���ݲ���ȡ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void W25N01G_Read(uint32_t addr, uint8_t buff[2048])
{
	uint32_t page_addr = addr >> 12;
	
	/* Transfer the data of specified page into the 2112-Byte Data Buffer */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PAGE_READ);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	while(W25N01G_FlashBusy()) __NOP();
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_FAST_READ);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	
	for(int i = 0; i < 2048; i++)
	{
		buff[i] = SPI_ReadWrite(SPI0, 0xFF);
	}
	
	W25N01G_Deassert();
}


void W25N01G_Read_4bit_DMA(uint32_t addr, uint8_t buff[2048])
{
	static bool dma_initialized = false;
	uint32_t page_addr = addr >> 12;
	
	/* Transfer the data of specified page into the 2112-Byte Data Buffer */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PAGE_READ);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	while(W25N01G_FlashBusy()) __NOP();
	
	
	if(!dma_initialized)
	{
		DMA_InitStructure DMA_initStruct;
		
		// SPI0 RX DMA
		DMA_initStruct.Mode = DMA_MODE_SINGLE;
		DMA_initStruct.Unit = DMA_UNIT_BYTE;
		DMA_initStruct.Count = 2048;
		DMA_initStruct.SrcAddr = (uint32_t)&SPI0->DATA;
		DMA_initStruct.SrcAddrInc = 0;
		DMA_initStruct.DstAddr = (uint32_t)buff;
		DMA_initStruct.DstAddrInc = 1;
		DMA_initStruct.Handshake = DMA_CH1_SPI0RX;
		DMA_initStruct.Priority = DMA_PRI_LOW;
		DMA_initStruct.INTEn = 0;
		DMA_CH_Init(DMA_CH1, &DMA_initStruct);
		DMA_CH_Open(DMA_CH1);
		
		dma_initialized = true;
	}
	else
	{
		DMA_CH_SetDstAddress(DMA_CH1, (uint32_t)buff);
		DMA_CH_Open(DMA_CH1);
	}
	
	SPI_Close(SPI0);
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (3 << SPI_CTRL_FFS_Pos);
	SPI0->SPIMCR = (1 << SPI_SPIMCR_EN_Pos) |
				   ((2048 - 1) << SPI_SPIMCR_RDLEN_Pos) |
				   ((8 - 1)   << SPI_SPIMCR_DUMMY_Pos);
	SPI_Open(SPI0);
	
	W25N01G_Assert();
	
	__disable_irq();
	
	SPI_Write(SPI0, (W25N_CMD_FAST_READ_4bit << 24) | 0x000000);
	while(SPI_IsRXEmpty(SPI0)) __NOP();
	buff[0] = SPI_Read(SPI0);
	
	SPI0->CTRL |= (1 << SPI_CTRL_DMARXEN_Pos);
	
	__enable_irq();
	
	while(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE) == 0) __NOP();
	DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
	
	W25N01G_Deassert();
	
	SPI0->CTRL &= ~(1 << SPI_CTRL_DMARXEN_Pos);
	
	SPI_Close(SPI0);
	SPI0->SPIMCR = 0;
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (0 << SPI_CTRL_FFS_Pos);
	SPI_Open(SPI0);
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_FlashBusy()
* ����˵��:	W25N01G æ���
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
bool W25N01G_FlashBusy(void)
{
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG3);
	
	bool busy = (reg & (1 << W25N_STATUS_REG3_BUSY_Pos));
	
	return busy;
}


/****************************************************************************************************************************************** 
* ��������:	W25N01G_FlashProtect()
* ����˵��:	W25N01G д��������
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void W25N01G_FlashProtect(uint8_t protect)
{
	uint8_t tb = (protect >> 4);
	uint8_t bp = (protect & 0xF);
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG1);
	
	reg &= ~(W25N_STATUS_REG1_TB_Msk | W25N_STATUS_REG1_BP_Msk | W25N_STATUS_REG1_WPE_Msk);
	reg |= (tb << W25N_STATUS_REG1_TB_Pos) |
		   (bp << W25N_STATUS_REG1_BP_Pos);
	
	W25N01G_WriteReg(W25N_STATUS_REG1, reg);
}


void W25N01G_WriteReg(uint8_t reg_addr, uint8_t data)
{	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_STATUS_REG);
	SPI_ReadWrite(SPI0, reg_addr);
	SPI_ReadWrite(SPI0, data);
	
	W25N01G_Deassert();
}


uint8_t W25N01G_ReadReg(uint8_t reg_addr)
{
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_READ_STATUS_REG);
	SPI_ReadWrite(SPI0, reg_addr);
	uint8_t data = SPI_ReadWrite(SPI0, 0xFF);
	
	W25N01G_Deassert();
	
	return data;
}
