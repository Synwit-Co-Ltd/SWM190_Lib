#include "SWM190.h"


void SerialInit(void);

int main(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PWM_initStruct.PWMnXN = 0;					//ֻ���PWM0A�źš������PWM0AN�ź�
	PWM_initStruct.clkdiv = PWM_CLKDIV_8;		//F_PWM = 24M/8 = 3M	
	PWM_initStruct.cycle = 10000;				//3M/10000 = 300Hz��PWMnXN = 1ʱƵ�ʽ��͵�150Hz
	PWM_initStruct.hduty =  2500;				//2500/10000 = 25%
	PWM_initStruct.deadzone = 00;
	PWM_initStruct.initLevel = 1;
	PWM_initStruct.HEndIE = 0;
	PWM_initStruct.NCycleIE = 0;
	PWM_initStruct.HCycleIE = 0;	
	PWM_Init(PWM0A, &PWM_initStruct);
	PWM_Init(PWM0B, &PWM_initStruct);
	PWM_Init(PWM1A, &PWM_initStruct);
	PWM_Init(PWM1B, &PWM_initStruct);
	
	PORT_Init(PORTB, PIN1, PORTB_PIN1_PWM0A,  0);
	PORT_Init(PORTB, PIN2, PORTB_PIN2_PWM0AN, 0);
	PORT_Init(PORTB, PIN3, PORTB_PIN3_PWM0B,  0);
	PORT_Init(PORTB, PIN4, PORTB_PIN4_PWM0BN, 0);
	PORT_Init(PORTB, PIN5, PORTB_PIN5_PWM1A,  0);
	PORT_Init(PORTB, PIN6, PORTB_PIN6_PWM1AN, 0);
	PORT_Init(PORTD, PIN2, PORTD_PIN2_PWM1B,  0);
	PORT_Init(PORTD, PIN3, PORTD_PIN3_PWM1BN, 0);
	
	PWM_Start(PWM0A);
	PWM_Start(PWM0B);
	PWM_Start(PWM1A);
	PWM_Start(PWM1B);
	PWMG->CHEN |= (1 << PWMG_CHEN_PWM0A_Pos) | (1 << PWMG_CHEN_PWM0B_Pos) | (1 << PWMG_CHEN_PWM1A_Pos) | (1 << PWMG_CHEN_PWM1B_Pos);	//��·ͬʱ����
	
	
	/* ɲ���������� */
	PORTB->PULLU |= (1 << PIN7);						//ʹ������
	PORT_Init(PORTB, PIN7, PORTB_PIN7_PWM_BRK0, 1);		//ɲ���ź�
	
	PWMG->BRKCR &= ~(1 << PWMG_BRKCR_S0INPOL_Pos);		//ɲ���źŵ͵�ƽ��Ч
	
	PWMG->BRKIE |= (1 << PWMG_BRKIE_S0_Pos);			//ɲ��ʱ�����ж�
	NVIC_EnableIRQ(GPIOE_PWMHALT_IRQn);
	
	PWM0->BRKEN = (1 << PWM_BRKEN_EN_Pos) |
				  (1 << PWM_BRKEN_S0_Pos) |
				  (0 << PWM_BRKEN_OUT_Pos);
	
	while(1==1)
	{
	
	}
}


void GPIOE_PWMHALT_Handler(void)
{
	if(PWMG->BRKIRS & PWMG_BRKIRS_S0_Msk)
	{
		PWMG->BRKIRS = (1 << PWMG_BRKIRS_S0_Pos);
		
		PWMG->BRKCR |= (1 << PWMG_BRKCR_S0STCLR_Pos);		//���ɲ���źţ������������
		
		printf("PWM Brake happened!\r\n");
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);	//GPIOA.1����ΪUART0 TXD
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	//GPIOA.0����ΪUART0 RXD
 	
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

