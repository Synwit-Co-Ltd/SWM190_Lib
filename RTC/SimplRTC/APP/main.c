#include "SWM190.h"


void SerialInit(void);

int main(void)
{
	RTC_InitStructure RTC_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	RTC_initStruct.clksrc = RTC_CLKSRC_LRC32K;
	RTC_initStruct.Year = 2016;
	RTC_initStruct.Month = 5;
	RTC_initStruct.Date = 5;
	RTC_initStruct.Hour = 15;
	RTC_initStruct.Minute = 5;
	RTC_initStruct.Second = 5;
	RTC_initStruct.SecondIEn = 1;
	RTC_initStruct.MinuteIEn = 0;
	RTC_Init(RTC, &RTC_initStruct);
	
	RTC_Start(RTC);
	
	while(1==1)
	{
	}
}


void GPIOD_RTC_Handler(void)
{
	RTC_DateTime dateTime;
	
	if(RTC_INTStat(RTC, RTC_IT_SECOND))
	{
		RTC_INTClr(RTC, RTC_IT_SECOND);
		
		RTC_GetDateTime(RTC, &dateTime);
		printf("Now Time: %d Minute %d Second\r\n", dateTime.Minute, dateTime.Second);
	}
	
	if(RTC_INTStat(RTC, RTC_IT_MINUTE))
	{
		RTC_INTClr(RTC, RTC_IT_MINUTE);
		
		RTC_GetDateTime(RTC, &dateTime);
		printf("Now Time: %d Minute %d Second\r\n", dateTime.Minute, dateTime.Second);
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
