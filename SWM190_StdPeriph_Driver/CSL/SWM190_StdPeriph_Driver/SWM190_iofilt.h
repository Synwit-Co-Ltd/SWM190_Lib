#ifndef __SWM190_IOFILT_H__
#define __SWM190_IOFILT_H__


/* 选择对哪个信号进行滤波 */
#define IOFILT0_PA15	0
#define IOFILT0_PC5		1
#define IOFILT0_PB5		2
#define IOFILT0_PA4		3

#define IOFILT1_PA14	0
#define IOFILT1_PC4		1
#define IOFILT1_PB6		2
#define IOFILT1_PA5		3


#define IOFILT_WIDTH_2		1
#define IOFILT_WIDTH_4		2
#define IOFILT_WIDTH_8		3
#define IOFILT_WIDTH_16		4
#define IOFILT_WIDTH_32		5
#define IOFILT_WIDTH_64		6
#define IOFILT_WIDTH_128	7
#define IOFILT_WIDTH_256	8
#define IOFILT_WIDTH_512	9
#define IOFILT_WIDTH_1024	10
#define IOFILT_WIDTH_2048	11
#define IOFILT_WIDTH_4096	12
#define IOFILT_WIDTH_8192	13
#define IOFILT_WIDTH_16384	14
#define IOFILT_WIDTH_32768	15



void IOFILT_Init(uint32_t IOFILTn, uint32_t signal, uint32_t width);
void IOFILT_Open(uint32_t IOFILTn);
void IOFILT_Close(uint32_t IOFILTn);


#endif // __SWM190_IOFILT_H__
