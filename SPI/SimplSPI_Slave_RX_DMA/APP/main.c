#include "SWM190.h"
#include "CircleBuffer.h"

CircleBuffer_t CirBuf;

#define RX_LEN	32
uint8_t RX_Buffer[2][RX_LEN] = {0};		// RX_Buffer[0]：DMA 前半段搬运使用的 Buffer
										// RX_Buffer[1]：DMA 后半段搬运使用的 Buffer


void SerialInit(void);
void SPIMstInit(void);
void SPISlvInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	SPIMstInit();
	SPISlvInit();
	
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, CyclesPerUs, 1000, 1);
	TIMR_Start(BTIMR0);
	
	while(1==1)
	{
		uint8_t chr;
		if(CirBuf_Read(&CirBuf, &chr, 1))
			printf("%c", chr);
	}
}


void BTIMR0_Handler(void)
{
	static int counter = 0;
	static char txbuffer[64] = "Hello from Synwit!\r\n";
	
	TIMR_INTClr(BTIMR0);
	
	counter += 1;
	
	if(counter == 1)
	{
		GPIO_ClrBit(GPIOA, PIN8);	// SPI_CS_Low()
	}
	else if(counter < 50)
	{
		SPI0->DATA = txbuffer[counter - 2];
	}
	else if(counter == 50)
	{
		GPIO_SetBit(GPIOA, PIN8);	// SPI_CS_High()
	}
	else if(counter == 1000)
	{
		counter = 0;
	}
}


void SPIMstInit(void)
{
	SPI_InitStructure SPI_initStruct;
	
//	PORT_Init(PORTA, PIN8,  PORTA_PIN8_SPI0_SSEL,  0);
	GPIO_Init(GPIOA, PIN8, 1, 0, 0, 0);					//软件控制片选
#define SPI_CS_Low()	GPIO_ClrBit(GPIOA, PIN8)
#define SPI_CS_High()	GPIO_SetBit(GPIOA, PIN8)
	SPI_CS_High();
	
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_SPI0_MISO,  1);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_SPI0_MOSI, 0);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_SPI0_SCLK, 0);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 4;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 4;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	SPI_Open(SPI0);
}


void SPISlvInit(void)
{
	SPI_InitStructure SPI_initStruct;
	DMA_InitStructure DMA_initStruct;
	
	PORT_Init(PORTC, PIN4, PORTC_PIN4_SPI1_SSEL, 1);
	PORT_Init(PORTC, PIN5, PORTC_PIN5_SPI1_MISO, 0);
	PORT_Init(PORTC, PIN6, PORTC_PIN6_SPI1_MOSI, 1);
	PORT_Init(PORTC, PIN7, PORTC_PIN7_SPI1_SCLK, 1);
	
	SPI_initStruct.clkDiv = 4;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 0;
	SPI_initStruct.RXThreshold = 4;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 4;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI1, &SPI_initStruct);
	SPI_Open(SPI1);
	

	// RX DMA
	SPI1->CTRL |= (1 << SPI_CTRL_DMARXEN_Pos);
	
	DMA_initStruct.Mode = DMA_MODE_CIRCLE;
	DMA_initStruct.Unit = DMA_UNIT_BYTE;
	DMA_initStruct.Count = RX_LEN * 2;	// DMA 搬运分两段，前半段和后半段各长 RX_LEN
	DMA_initStruct.SrcAddr = (uint32_t)&SPI1->DATA;
	DMA_initStruct.SrcAddrInc = 0;
	DMA_initStruct.DstAddr = (uint32_t)RX_Buffer;
	DMA_initStruct.DstAddrInc = 2;		// Scatter-Gather 模式
	DMA_initStruct.Trigger = DMA_CH0_SPI1RX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = DMA_IT_DSTSG_HALF | DMA_IT_DSTSG_DONE;
	DMA_CH_Init(DMA_CH0, &DMA_initStruct);
	DMA_CH_Open(DMA_CH0);
}


void DMA_Handler(void)
{	
	if(DMA_CH_INTStat(DMA_CH0, DMA_IT_DSTSG_HALF))
	{
		DMA_CH_INTClr(DMA_CH0, DMA_IT_DSTSG_HALF);
		
		CirBuf_Write(&CirBuf, RX_Buffer[0], RX_LEN);
	}
	else if(DMA_CH_INTStat(DMA_CH0, DMA_IT_DSTSG_DONE))
	{
		DMA_CH_INTClr(DMA_CH0, DMA_IT_DSTSG_DONE);
		
		CirBuf_Write(&CirBuf, RX_Buffer[1], RX_LEN);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);		//GPIOA.1配置为UART0 TXD
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);		//GPIOA.0配置为UART0 RXD
 	
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
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
