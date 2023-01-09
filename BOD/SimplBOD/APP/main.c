#include "SWM190.h"


int main(void)
{	
	SystemInit();
	
	SYS->BODSR = (1 << SYS_BODSR_IF_Pos);
	SYS->BODCR = (1 << SYS_BODCR_IE_Pos);	//�����ѹ����2.7vʱ�ж�
	NVIC_EnableIRQ(GPIOB_BOD_IRQn);
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);		//����� ��LED
	
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN5);
	}
}


void GPIOB_BOD_Handler(void)
{
	SYS->BODSR = (1 << SYS_BODSR_IF_Pos);
	
	GPIO_SetBit(GPIOA, PIN5);
}
