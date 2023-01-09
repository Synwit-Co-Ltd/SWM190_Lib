#include "SWM190.h"

uint16_t *SPI1TXBuff = 0;
volatile uint32_t SPI1TXCount = 0;
volatile uint32_t SPI1TXIndex = 0;


void SerialInit(void);
void SPIMstInit(void);
void SPISlvInit(void);
void SPISlvSend(uint16_t buff[], uint32_t cnt);

int main(void)
{
	uint32_t i, j;
	uint16_t rxbuff[16] = {0};
	uint16_t txbuff[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	
	SystemInit();
	
	SerialInit();	
	
	SPIMstInit();
	
	SPISlvInit();
	
	SPISlvSend(txbuff, 16);

	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN8);	// SPI0_CS_Low()
		for(i = 0; i < 240; i++);	// CS���ͺ���Ҫ��ʱһ���ٷ���
		for(i = 0, j = 0; i < 16; i++)
		{
			SPI0->DATA = 0xFF;
			while((SPI0->STAT & SPI_STAT_TFNF_Msk) == 0);
			
			if(SPI0->STAT & SPI_STAT_RFNE_Msk) rxbuff[j++] = SPI0->DATA;
		}
		while(j < 16)
		{
			if(SPI0->STAT & SPI_STAT_RFNE_Msk) rxbuff[j++] = SPI0->DATA;
		}
		GPIO_SetBit(GPIOA, PIN8);	// SPI0_CS_High()
		
		for(i = 0; i < 16; i++) printf("%d, ", rxbuff[i]);
		printf("\r\n\r\n");
		
		for(i = 0; i < SystemCoreClock/4; i++);
	}
}


void SPIMstInit(void)
{
	SPI_InitStructure SPI_initStruct;
	
//	PORT_Init(PORTA, PIN8,  PORTA_PIN8_SPI0_SSEL,  0);
	GPIO_Init(GPIOA, PIN8, 1, 0, 0, 0);					//�������Ƭѡ
#define SPI0_CS_Low()	GPIO_ClrBit(GPIOA, PIN8)
#define SPI0_CS_High()	GPIO_SetBit(GPIOA, PIN8)
	SPI0_CS_High();
	
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_SPI0_MISO,  1);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_SPI0_MOSI, 0);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_SPI0_SCLK, 0);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 0;
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
	
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 0;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 4;		//����FIFO�����ݸ�����CTRL.TFTHRʱ�����ж�
	SPI_initStruct.TXThresholdIEn = 1;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI1, &SPI_initStruct);
	
	SPI_Open(SPI1);
	
	
	/* ��PC4�������������ڼ��SPI�ӻ���CS Assert�¼� */
	GPIO_Init(GPIOA, PIN4, 0, 1, 0, 0);			//���룬����ʹ��
	
	EXTI_Init(GPIOA, PIN4, EXTI_FALL_EDGE);		//�½��ش����ж�
	
	NVIC_EnableIRQ(GPIOA4_IRQn);
		
	EXTI_Open(GPIOA, PIN4);
}

void SPISlvSend(uint16_t buff[], uint32_t cnt)
{
	SPI1TXBuff = buff;
	SPI1TXCount = cnt;
}

void SPI1_BTIMR1_Handler(void)
{
	if(SPI1->IF & SPI_IF_TFTHR_Msk)
	{
		while(SPI1->STAT & SPI_STAT_TFNF_Msk)
		{
			if(SPI1TXIndex < SPI1TXCount)
			{
				SPI1->DATA = SPI1TXBuff[SPI1TXIndex++];
			}
			else
			{
				SPI1->DATA = 0xFF;	// û�����ˣ�����0xFF
			}
		}
		
		SPI1->IF = (1 << SPI_IF_TFTHR_Pos);
	}
}

void GPIOA4_Handler(void)
{	
	EXTI_Clear(GPIOA, PIN4);
	
	SPI1TXIndex = 0;
	
	SPI1->CTRL |=  (1 << SPI_CTRL_TFCLR_Pos);	// ����FIFO���
	SPI1->CTRL &= ~(1 << SPI_CTRL_TFCLR_Pos);
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
