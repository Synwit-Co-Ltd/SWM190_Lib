#include "SWM190.h"


void SerialInit(void);

int main(void)
{
	uint32_t addr;
	
	/* ע�⣺
	*  ִ��CACHE_Unlock(addr)ʱ��CPU���ܷ���addr��ַ�Ĵ��룬��������δ����CACHE_Prefet()��CACHE_Unlock()��������������Ҫ����ĵ�ַ��Χ��
	*  ���ԣ���ý�������δ����Լ�CACHE_Prefet()��CACHE_Unlock()����������λ�������β�������߿�����RAM��
	***************************************/
	for(addr = 0; addr < 256; addr += 128)	// ������ǰ256�ֽ�������Cache�У��ڷ�ɢ�����ļ��н���������жϷ������λ������ǰ��
	{
		uint32_t res;
		
		do {
			res = CACHE_Prefet(addr);
			
			if(res == CACHE_RES_FAIL) CACHE_Unlock(addr);
		} while(res == CACHE_RES_FAIL);
	}
	
	SystemInit();
	
	SerialInit();
	
	SysTick_Config(SystemCoreClock / 10);
   	
	while(1==1)
	{
	}
}

__attribute__((section("..LockInCache")))
void SysTick_Handler(void)
{
	
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
