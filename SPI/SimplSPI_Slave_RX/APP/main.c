#include "SWM190.h"

#include <string.h>

uint16_t SPI1RXBuffer[32] = {0};
volatile uint32_t SPI1RXIndex = 0;

void SerialInit(void);
void SPIMstInit(void);
void SPISlvInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();	
	
	SPIMstInit();
	SPISlvInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0 ,0);
	GPIO_ClrBit(GPIOA, PIN5);
	
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN8);	// SPI_CS_Low()
		for(i = 0; i < 240; i++);	// CS���ͺ���Ҫ��ʱһ���ٷ���
		for(i = 0; i < 16; i++)
		{
			SPI0->DATA = i;
			while(SPI_IsTXFull(SPI0)) __NOP();
		}
		while(SPI_IsTXEmpty(SPI0) == 0);
		for(i = 0; i < 120; i++);	// ����FIFO���ѿգ������һ�����ݻ��ڷ�����λ�Ĵ������Ҫ��ʱ�ȴ������ͳ�ȥ
		GPIO_SetBit(GPIOA, PIN8);	// SPI_CS_High()
		
		for(i = 0; i < SystemCoreClock/10; i++);
	}
}

void SPIMstInit(void)
{
	SPI_InitStructure SPI_initStruct;
	
//	PORT_Init(PORTA, PIN8,  PORTA_PIN8_SPI0_SSEL,  0);
	GPIO_Init(GPIOA, PIN8, 1, 0, 0, 0);					//�������Ƭѡ
#define SPI_CS_Low()	GPIO_ClrBit(GPIOA, PIN8)
#define SPI_CS_High()	GPIO_SetBit(GPIOA, PIN8)
	SPI_CS_High();
	
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_SPI0_MISO,  1);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_SPI0_SCLK, 0);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_SPI0_MOSI, 0);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_64;
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
	SPI_initStruct.RXThreshold = 4;		//����FIFO�����ݸ�����CTRL.RFTHRʱ�����ж�
	SPI_initStruct.RXThresholdIEn = 1;
	SPI_initStruct.TXThreshold = 4;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI1, &SPI_initStruct);
	
	SPI_Open(SPI1);
	
	
	/* ��PC4�������������ڼ��SPI�ӻ���CS Deassert�¼� */
	GPIO_Init(GPIOA, PIN4, 0, 1, 0, 0);			//���룬����ʹ��
	
	EXTI_Init(GPIOA, PIN4, EXTI_RISE_EDGE);		//�����ش����ж�
	
	NVIC_EnableIRQ(GPIOA4_IRQn);
	
	EXTI_Open(GPIOA, PIN4);
}

void SPI1_BTIMR1_Handler(void)
{
	if(SPI1->IF & SPI_IF_RFTHR_Msk)
	{
		GPIO_InvBit(GPIOA, PIN5);
		
		while(SPI1->STAT & SPI_STAT_RFNE_Msk)
		{
			SPI1RXBuffer[SPI1RXIndex++] = SPI1->DATA;
		}
		
		SPI1->IF = (1 << SPI_IF_RFTHR_Pos);	// �ȶ������ݣ��������־
	}
}

void GPIOA4_Handler(void)
{	
	uint32_t i;
	
	EXTI_Clear(GPIOA, PIN4);
	
	while(SPI1->STAT & SPI_STAT_RFNE_Msk)
	{
		SPI1RXBuffer[SPI1RXIndex++] = SPI1->DATA;
	}
	
	for(i = 0; i < 16; i++) printf("%d, ", SPI1RXBuffer[i]);
	printf("\r\n\r\n");
	
	SPI1RXIndex = 0;
	memset(SPI1RXBuffer, 0, 64);
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);		//GPIOA.1����ΪUART0 TXD
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);		//GPIOA.0����ΪUART0 RXD
 	
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
