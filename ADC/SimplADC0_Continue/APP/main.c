#include "SWM190.h"

uint16_t Buffer[1000] = {0};

void SerialInit(void);

int main(void)
{
	uint32_t i;
	ADC_InitStructure ADC_initStruct;
	
	SystemInit();
	
	SerialInit();   
	
	PORT_Init(PORTE, PIN4,  PORTE_PIN4_ADC0_IN0,  0);		//PE.4  => ADC0.CH0
	PORT_Init(PORTA, PIN15, PORTA_PIN15_ADC0_IN1, 0);		//PA.15 => ADC0.CH1
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_IN2, 0);		//PA.14 => ADC0.CH2
	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC0_IN3, 0);		//PA.13 => ADC0.CH3
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);		//PA.12 => ADC0.CH4
	PORT_Init(PORTC, PIN7,  PORTC_PIN7_ADC0_IN5,  0);		//PC.7  => ADC0.CH5
	PORT_Init(PORTC, PIN6,  PORTC_PIN6_ADC0_IN6,  0);		//PC.6  => ADC0.CH6
	
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV8;
	ADC_initStruct.channels = ADC_CH0;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_initStruct.Continue = 1;						//非连续模式，即单次模式
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//配置ADC
		
	ADC_Open(ADC0);										//使能ADC
	
	while(1==1)
	{
		ADC_Start(ADC0);
		
		for(i = 0; i < 1000; i++)
		{
			while((ADC0->CH[0].STAT & ADC_STAT_EOC_Msk) == 0);
			ADC0->CH[0].STAT = (1 << ADC_STAT_EOC_Pos);
			Buffer[i] = ADC0->CH[0].DATA;
		}
		
		ADC_Stop(ADC0);
		
		for(i = 0; i < 1000; i++)
			printf("%d,", Buffer[i]);
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
