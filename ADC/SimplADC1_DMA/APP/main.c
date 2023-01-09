#include "SWM190.h"

#define ADC_SIZE  1000
uint16_t ADC_Result[ADC_SIZE] = {0};


void SerialInit(void);

int main(void)
{
	ADC_InitStructure ADC_initStruct;
	DMA_InitStructure DMA_initStruct;
	
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
	ADC_initStruct.channels = ADC_CH0 | ADC_CH1;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_initStruct.Continue = 1;						//����ģʽ
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC1, &ADC_initStruct);					//����ADC
	
	ADC_Open(ADC1);										//ʹ��ADC
	
	ADC1->CTRL |= (1 << ADC_CTRL_RES2FIFO_Pos) | (1 << ADC_CTRL_DMAEN_Pos);
	
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_HALFWORD;
	DMA_initStruct.Count = ADC_SIZE;
	DMA_initStruct.SrcAddr = (uint32_t)&ADC1->FIFODR;
	DMA_initStruct.SrcAddrInc = 0;
	DMA_initStruct.DstAddr = (uint32_t)ADC_Result;
	DMA_initStruct.DstAddrInc = 1;
	DMA_initStruct.Trigger = DMA_CH1_ADC1;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = DMA_IT_DONE;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	
	DMA_CH_Open(DMA_CH1);
	
	ADC_Start(ADC1);
	
	while(1==1)
	{
	}
}


void DMA_Handler(void)
{
	uint32_t i;
	
	if(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE))
	{
		DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
		
		ADC_Stop(ADC1);
		
		for(i = 0; i < ADC_SIZE; i++)
		{
			if(((ADC_Result[i] & ADC_FIFDR_NUM_Msk) >> ADC_FIFDR_NUM_Pos) == 0)		// ͨ��0
			{
				printf("%4d,", ADC_Result[i] & ADC_FIFDR_VAL_Msk);
			}
// 			else if(((ADC_Result[i] & ADC_FIFDR_NUM_Msk) >> ADC_FIFDR_NUM_Pos) == 1)	// ͨ��1
// 			{
// 				printf("%4d,", ADC_Result[i] & ADC_FIFDR_VAL_Msk);
// 			}
		}
		
		DMA_CH_Open(DMA_CH1);	// ���¿�ʼ���ٴΰ���
		
		ADC_Start(ADC1);
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
