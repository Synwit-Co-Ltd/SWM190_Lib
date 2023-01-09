#include "SWM190.h"

void SerialInit(void);

int main(void)
{
	ADC_InitStructure ADC_initStruct;
	
	SystemInit();
	
	SerialInit();   	
	
	PORT_Init(PORTD, PIN1,  PORTD_PIN1_ADC1_IN0,  0);	//PD.1 => ADC1.CH0
	PORT_Init(PORTD, PIN0,  PORTD_PIN0_ADC1_IN1,  0);	//PD.0 => ADC1.CH1
	PORT_Init(PORTC, PIN5,  PORTC_PIN5_ADC1_IN2,  0);	//PC.5 => ADC1.CH2
	PORT_Init(PORTC, PIN4,  PORTC_PIN4_ADC1_IN3,  0);	//PC.4 => ADC1.CH3
	PORT_Init(PORTC, PIN3,  PORTC_PIN3_ADC1_IN4,  0);	//PC.3 => ADC1.CH4
	PORT_Init(PORTC, PIN2,  PORTC_PIN2_ADC1_IN5,  0);	//PC.2 => ADC1.CH5
	PORT_Init(PORTB, PIN15, PORTB_PIN15_ADC1_IN6, 0);	//PB.15=> ADC1.CH6
	PORT_Init(PORTB, PIN14, PORTB_PIN14_ADC1_IN7, 0);	//PB.14=> ADC1.CH7
	
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV8;
	ADC_initStruct.channels = ADC_CH0 | ADC_CH1 | ADC_CH2 | ADC_CH3 | ADC_CH4 | ADC_CH5 | ADC_CH6;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_initStruct.Continue = 0;						//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC1, &ADC_initStruct);					//����ADC
	
	ADC_Open(ADC1);										//ʹ��ADC
	
	while(1==1)
	{
		ADC_Start(ADC1);
		while((ADC1->CH[6].STAT & 0x01) == 0);
		printf("%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\r\n", ADC_Read(ADC1, ADC_CH0), ADC_Read(ADC1, ADC_CH1), ADC_Read(ADC1, ADC_CH2), ADC_Read(ADC1, ADC_CH3),
													    ADC_Read(ADC1, ADC_CH4), ADC_Read(ADC1, ADC_CH5), ADC_Read(ADC1, ADC_CH6));
	}
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
