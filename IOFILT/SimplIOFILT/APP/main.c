#include "SWM190.h"

void SerialInit(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
   	
	/* ����˵����
		�� PA5 �� PB5 ���ӣ�ʾ�����۲� PA5 �� PB6 ���Σ��ɹ۲쵽 PA5 �ϽϿ������屻���Ƶ��� PB6 �ϣ�����խ�Ĳ���û��
	*/
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);
	
	GPIO_Init(GPIOB, PIN5, 0, 1, 0, 0);
	EXTI_Init(GPIOB, PIN5, EXTI_BOTH_EDGE);	//˫���ش����ж�
	NVIC_EnableIRQ(GPIOB_BOD_IRQn);
	EXTI_Open(GPIOB, PIN5);
	
	GPIO_Init(GPIOB, PIN6, 1, 0, 0, 0);
	
	IOFILT_Init(0, IOFILT0_PB5, IOFILT_WIDTH_32);
   	
	while(1==1)
	{
		int i;
		
		GPIO_InvBit(GPIOA, PIN5);
		for(i = 0; i < 2; i++) __NOP();
		
		GPIO_InvBit(GPIOA, PIN5);
		for(i = 0; i < 20; i++) __NOP();
		
		GPIO_InvBit(GPIOA, PIN5);
		for(i = 0; i < 200; i++) __NOP();
		
		GPIO_InvBit(GPIOA, PIN5);
		for(i = 0; i < 2000; i++) __NOP();
	}
}


void GPIOB_BOD_Handler(void)
{
	EXTI_Clear(GPIOB, PIN5);
	
	GPIOB->DATAPIN6 = GPIOB->DATAPIN5;
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