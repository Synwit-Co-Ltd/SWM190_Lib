#ifndef __SYSTEM_SWM190_H__
#define __SYSTEM_SWM190_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define SYS_CLK_24MHz		0	 	//0 �ڲ���Ƶ24MHz RC����
#define SYS_CLK_48MHz		2		//2 �ڲ���Ƶ48MHz RC����
#define SYS_CLK_XTAL		4		//4 �ⲿ����������2-30MHz��


#define PLL_OUT_DIV8		0
#define PLL_OUT_DIV4		1
#define PLL_OUT_DIV2		2


extern uint32_t SystemCoreClock;		// System Clock Frequency (Core Clock)
extern uint32_t CyclesPerUs;			// Cycles per micro second


extern void SystemInit(void);

extern void SystemCoreClockUpdate (void);


extern void switchTo24MHz(void);
extern void switchTo3MHz(void);
extern void switchTo48MHz(void);
extern void switchTo6MHz(void);
extern void switchToXTAL(uint32_t div8);
extern void switchToPLL(uint32_t clksrc, uint32_t indiv, uint32_t fbdiv, uint32_t outdiv, uint32_t div8);
extern void switchTo32KHz(void);
extern void switchToXTAL_32K(void);


#ifdef __cplusplus
}
#endif

#endif //__SYSTEM_SWM190_H__
