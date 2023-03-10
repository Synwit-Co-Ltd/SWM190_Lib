#include "SWM190.h"

#include "ucos_ii.h"

/*  说明：为了避免修改 startup_SWM181.s 中的向量表，将：
	os_cpu_a.asm、os_cpu.h 中的 OS_CPU_PendSVHandler 修改为 PendSV_Handler
	os_cpu_c.c、os_cpu.h 中的 OS_CPU_SysTickHandler 修改为 SysTick_Handler
*/

OS_EVENT *queueADC;
void *queueADCTbl[16];

static  OS_STK TaskADCStk[TASK_ADC_STK_SIZE];
static  OS_STK TaskPWMStk[TASK_PWM_STK_SIZE];

void TaskADC(void *arg);
void TaskPWM(void *arg);
void SerialInit(void);

int main(void)
{ 	
 	SystemInit();
	
	SerialInit();
	
	OSInit();
	
	queueADC = OSQCreate(&queueADCTbl[0], 16);
	
	OSTaskCreate((void (*)(void *))TaskADC, (void *)0, &TaskADCStk[TASK_ADC_STK_SIZE - 1], TASK_ADC_PRIO);
	OSTaskCreate((void (*)(void *))TaskPWM, (void *)0, &TaskPWMStk[TASK_PWM_STK_SIZE - 1], TASK_PWM_PRIO);
	
	OS_CPU_SysTickInit(SystemCoreClock / OS_TICKS_PER_SEC);
	OSStart();
}

/****************************************************************************************************************************************** 
* 函数名称:	TaskADC()
* 功能说明: 启动ADC采集任务
* 输    入: void *arg
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void TaskADC(void *arg)
{
	ADC_InitStructure ADC_initStruct;
	
	PORT_Init(PORTE, PIN4,  PORTE_PIN4_ADC0_IN0,  0);		//PE.4  => ADC0.CH0
	PORT_Init(PORTA, PIN15, PORTA_PIN15_ADC0_IN1, 0);		//PA.15 => ADC0.CH1
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_IN2, 0);		//PA.14 => ADC0.CH2
	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC0_IN3, 0);		//PA.13 => ADC0.CH3
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);		//PA.12 => ADC0.CH4
	PORT_Init(PORTC, PIN7,  PORTC_PIN7_ADC0_IN5,  0);		//PC.7  => ADC0.CH5
	PORT_Init(PORTC, PIN6,  PORTC_PIN6_ADC0_IN6,  0);		//PC.6  => ADC0.CH6
	
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC_DIV8;
	ADC_initStruct.channels = ADC_CH3;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE2;
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_initStruct.Continue = 0;						//非连续模式，即单次模式
	ADC_initStruct.EOC_IEn = ADC_CH3;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//配置ADC
	
	NVIC_SetPriority(GPIOC4_ADC0_IRQn, 2);
	
	ADC_Open(ADC0);										//使能ADC
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);					//调试指示信号
	
	while(1)
	{
		ADC_Start(ADC0);
		
		OSTimeDly(OS_TICKS_PER_SEC/5);
	}
}

void GPIOC4_ADC0_Handler(void)
{
	uint32_t val;
	
	ADC0->IF = (1 << ADC_IF_CH3EOC_Pos);
	
	val = ADC_Read(ADC0, ADC_CH3);
	
	OSQPost(queueADC, (void *)val);
	
	GPIO_InvBit(GPIOA, PIN5);
}


/****************************************************************************************************************************************** 
* 函数名称:	TaskPWM()
* 功能说明: 等待ADC转换结果，根据ADC转换结果设置PWM占空比
* 输    入: void *arg
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void TaskPWM(void *arg)
{
	PWM_InitStructure  PWM_initStruct;
	
	PWM_initStruct.PWMnXN = 0;					//只输出PWM0A信号、不输出PWM0AN信号
	PWM_initStruct.clkdiv = PWM_CLKDIV_8;		//F_PWM = 24M/8 = 3M	
	PWM_initStruct.cycle = 10000;				//3M/10000 = 300Hz，PWMnXN = 1时频率降低到150Hz
	PWM_initStruct.hduty =  2500;				//2500/10000 = 25%
	PWM_initStruct.deadzone = 00;
	PWM_initStruct.initLevel = 1;
	PWM_initStruct.HEndIE = 0;
	PWM_initStruct.NCycleIE = 0;
	PWM_initStruct.HCycleIE = 0;	
	PWM_Init(PWM0A, &PWM_initStruct);
	PWM_Init(PWM0B, &PWM_initStruct);
	
	PORT_Init(PORTB, PIN1, PORTB_PIN1_PWM0A,  0);
	PORT_Init(PORTB, PIN2, PORTB_PIN2_PWM0AN, 0);
	PORT_Init(PORTB, PIN3, PORTB_PIN3_PWM0B,  0);
	PORT_Init(PORTB, PIN4, PORTB_PIN4_PWM0BN, 0);
	
	PWM_Start(PWM0A);
	PWM_Start(PWM0B);
	
	while(1)
	{
		uint8_t err;
		uint32_t duty;
		
		duty = (uint32_t)OSQPend(queueADC, 10, &err);
		if(err == OS_ERR_NONE)
		{
			printf("%d,", duty);
			if(duty <  100) duty =  100;
			if(duty > 4000) duty = 4000;
			
			PWM_SetHDuty(PWM0A, 10000 * duty / 4095);
			PWM_SetHDuty(PWM0B, 10000 - PWM_GetHDuty(PWM0A));
		}
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);		//GPIOA.1配置为UART0 TXD
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);		//GPIOA.0配置为UART0 RXD
 	
 	UART_initStruct.Baudrate = 57600;
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
 	while(UART_IsTXFIFOFull(UART0));
	
	UART_WriteByte(UART0, ch);
 	
	return ch;
}
