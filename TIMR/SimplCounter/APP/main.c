#include "SWM190.h"


int main(void)
{	
	uint32_t i;
	SystemInit();
	
	GPIO_Init(GPIOA, PIN4, 1, 0, 0, 0);			//�Ӽ��������룬����GPIOA.4������
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//GPIOA.5 => LED
	
	PORT_Init(PORTA, PIN7, PORTA_PIN7_TIMR2_IN, 1);
	PORTA->PULLU |= (1 << PIN7);
	
	TIMR_Init(TIMR2, TIMR_MODE_COUNTER, 1, 3, 1);	//ÿ��3�������ؽ����ж�
	TIMR_Start(TIMR2);
	
	while(1==1)
	{
		GPIO_InvBit(GPIOA, PIN4);
		for(i = 0; i < 10000; i++) __NOP();
	}
}

void TIMR2_GPIOA15_Handler(void)
{
	TIMR_INTClr(TIMR2);
	
	GPIO_InvBit(GPIOA, PIN5);
}
