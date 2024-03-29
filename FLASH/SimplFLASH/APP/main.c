#include "SWM190.h"


/* 注意：SWM190_flash.c 中的函数必须在RAM中执行，以防止 Flash 擦写时被 Cache 读取，Keil下实现方法有：
   方法一、Scatter file
   方法二、SWM190_flash.c上右键 =》Options for File "SWM190_flash.c" =》Properties =》Memory Assignment =》Code/Conts 选择 IRAM1
*/


#define EEPROM_ADDR	  0x10000

uint32_t WrBuff[20] = {0x14141414, 0x15151515, 0x16161616, 0x17171717, 0x18181818,
				       0x19191919, 0x1A1A1A1A, 0x1B1B1B1B, 0x1C1C1C1C, 0x1D1D1D1D,
				       0x1E1E1E1E, 0x1F1F1F1F, 0x20202020, 0x21212121, 0x22222222,
				       0x23232323, 0x24242424, 0x25252525, 0x26262626, 0x27272727};
uint32_t RdBuff[20] = {0};


void SerialInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
		
	printf("\r\nAfter Erase: \r\n");
	
	FLASH_Erase(EEPROM_ADDR);
	
	FLASH_Read(EEPROM_ADDR, RdBuff, 20);
	for(i = 0; i < 20; i++) printf("0x%X, ", RdBuff[i]);
	
	printf("\r\nAfter Write: \r\n");
   	
	FLASH_Write(EEPROM_ADDR, WrBuff, 20);
	
	FLASH_Read(EEPROM_ADDR, RdBuff, 20);
	for(i = 0; i < 20; i++) printf("0x%X, ", RdBuff[i]);
   	
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
