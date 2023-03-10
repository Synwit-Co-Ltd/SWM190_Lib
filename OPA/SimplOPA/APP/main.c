#include "SWM190.h"

void SerialInit(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN1,  PORTA_PIN1_OPA0_INP,  0);
	PORT_Init(PORTA, PIN6,  PORTA_PIN6_OPA0_INN,  0);
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_OPA0_OUT,  0);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_OPA1_INN,  0);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_OPA1_INP,  0);
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_OPA1_OUT,  0);
	PORT_Init(PORTA, PIN13, PORTA_PIN13_OPA2_INN, 0);
	PORT_Init(PORTA, PIN14, PORTA_PIN14_OPA2_INP, 0);
	PORT_Init(PORTD, PIN7,  PORTD_PIN7_OPA2_OUT,  0);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_OPA3_INN, 0);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_OPA3_INP, 0);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_OPA3_OUT,  0);
	
	SYS->ACMPCR |= (1 << SYS_ACMPCR_CMP0MODE_Pos);	//放大器模式
	SYS->OPACR  |= (1 << SYS_OPACR_OPA0ON_Pos);
	SYS->ACMPCR |= (1 << SYS_ACMPCR_CMP1MODE_Pos);	//放大器模式
	SYS->OPACR  |= (1 << SYS_OPACR_OPA1ON_Pos);
	SYS->ACMPCR |= (1 << SYS_ACMPCR_CMP2MODE_Pos);	//放大器模式
	SYS->OPACR  |= (1 << SYS_OPACR_OPA2ON_Pos);
	SYS->OPACR  |= (1 << SYS_OPACR_OPA3ON_Pos);
	
	while(1==1)
	{
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
