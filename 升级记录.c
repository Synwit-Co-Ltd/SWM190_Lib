2018/9/26
CSL\CMSIS\DeviceSupport\SWM190.h文件中struct RTC_TypeDef定义中LOAD寄存器添加位定义
CSL\SWM190_StdPeriph_Driver\SWM190_rtc.c文件中RTC_Init()、RTC_AlarmSetup()中RTC->LOAD寄存器操作修正

2018/10/19
CSL\SWM190_StdPeriph_Driver\SWM190_adc.c文件中ADC_IntEOCEn()函数中将
ADCx->IE |= (0x01 << (idx*4));
改为：
ADCx->IE |= (0x01 << (idx*2));
其后七个相关函数也有类似修正

2018/10/30
CSL\SWM190_StdPeriph_Driver\SWM190_spi.c文件中SPI_Init()函数中将
((initStruct->RXThreshold-1) << SPI_CTRL_RFTHR_Pos)  |
改正为：
((initStruct->RXThreshold > 0 ? initStruct->RXThreshold-1 : 0) << SPI_CTRL_RFTHR_Pos)  |

2018/12/28
CSL\SWM190_StdPeriph_Driver\SWM190_uart.c文件中 UART_IsTXBusy() 函数代码由
return (UARTx->CTRL & UART_CTRL_TXIDLE_Msk) ? 0 : 1;
改正为：
if(UARTx->FIFO & UART_FIFO_TXLVL_Msk)
{
	return 1;
}
else
{
	if(((UARTx->BAUD & UART_BAUD_TXD_Msk) == 0) || ((UARTx->CTRL & UART_CTRL_TXIDLE_Msk) == 0))
		return 1;
	else
		return 0;
}

2019/01/11
CSL\CMSIS\DeviceSupport\startup\arm\startup_SWM190.s 文件添加QSPI模式切换代码

2019/01/16
Timr Hall功能输入切换到其他引脚，添加输入引脚状态寄存器
UART添加波特率小数分频，输入、输出引脚可分别翻转电平；超时功能增加模式选择、扩展最长超时时间

2019/01/17
CSL\CMSIS\DeviceSupport\SWM190.h文件中 DIV->CR 寄存器添加 SIGN 位
CSL\SWM190_StdPeriph_Driver\SWM190_div.h文件中，将DIV_Div() 分成 DIV_UDiv() 和 DIV_SDiv()

2019/02/13
CSL\SWM190_StdPeriph_Driver\SWM190_exti.c 文件中 EXTI_RawState() 中，将
return (GPIOx->INTRAWSTAT >> 1) & 0x01;
修正为：
return (GPIOx->INTRAWSTAT >> n) & 0x01;

2019/02/13
CSL\SWM190_StdPeriph_Driver\SWM190_gpio.c 文件中，将
bits = 0xFFFFFF >> (24 - w);
修正为：
bits = 0xFFFF >> (16 - w);

2019/02/14
CSL\SWM190_StdPeriph_Driver\SWM190_uart.c 文件中，UART_SetBaudrate()修正，添加小数分频

2019/03/04
CSL\SWM190_StdPeriph_Driver\SWM190_i2c.c 文件中，I2C_Init()中将：
if(initStruct->MstIEn)  I2Cx->MSTCR &= ~(1 << I2C_MSTCR_IE_Pos);
else                    I2Cx->MSTCR |=  (1 << I2C_MSTCR_IE_Pos);
修正为：
if(initStruct->MstIEn)  I2Cx->MSTCR |=  (1 << I2C_MSTCR_IE_Pos);
else                    I2Cx->MSTCR &= ~(1 << I2C_MSTCR_IE_Pos);

CSL\SWM190_StdPeriph_Driver\SWM190_timr.c 文件中，TIMR_INTClr()中将：
TIMRx->IF |= (1 << TIMR_IF_TO_Pos);
修正为：
TIMRx->IF = (1 << TIMR_IF_TO_Pos);

CSL\SWM190_StdPeriph_Driver\SWM190_uart.c 文件中，UART_GetBaudrate()中将：
return (UARTx->BAUD & UART_BAUD_BAUD_Msk);
修正为：
return SystemCoreClock/(((UARTx->BAUD & UART_BAUD_BAUD_Msk) >> UART_BAUD_BAUD_Pos) * 16 +
                        ((UARTx->BAUD & UART_BAUD_FRAC_Msk) >> UART_BAUD_FRAC_Pos) + 1);

CSL\SWM190_StdPeriph_Driver\SWM190_port.h 文件中，将：
#define PORTB_PIN15_SPI0_SCLK	1
修正为：
#define PORTB_PIN15_I2C0_SCL	1

#define PORTA_PIN12_PWM1A		3
修正为：
#define PORTA_PIN12_PWM1AN		3

2019/03/08
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，CLKSEL寄存器的bit5名称由LPR改为RTC

CSL\SWM190_StdPeriph_Driver\SWM190_rtc.c 文件中，RTC_Init()中添加时钟源选择

2019/03/12
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，SYS_TypeDef 定义中将 CHIPID[4] 修正为 CHIPID[3]

CSL\CMSIS\DeviceSupport\startup\arm\startup_SWM190.s 文件中，Reset_Handler 定义中修改 flash 设定代码

2019/04/18
CSL\CMSIS\DeviceSupport\startup\arm\startup_SWM190.s 文件中，将TIMR4、TIMR5、TIMR6、TIMR7更正为BTIMR0、BTIMR1、BTIMR2、BTIMR3

2019/05/15
CSL\SWM190_StdPeriph_Driver\SWM190_rtc.c 文件中，修正：Date取值1--31，Month取值1--12

2019/05/20
CSL\CMSIS\DeviceSupport\SWM190.h 文件中 PWMG->FORCEH 寄存器定义更正
CSL\CMSIS\DeviceSupport\system_SWM190.c 文件中，删除 SystemCoreClockUpdate() 中如下行：
if(SystemCoreClock > 48000000)  *((volatile uint32_t *) 0x4004A010) |= (2 << 16);	//Flash适应72MHz

2019/07/31
CSL\SWM190_StdPeriph_Driver\SWM190_uart.c 文件中 UART_LINIsDetected() 函数中，将：
return (UARTx->LINCR & UART_LINCR_BRKDETIE_Msk) ? 1 : 0;
修正为：
return (UARTx->LINCR & UART_LINCR_BRKDETIF_Msk) ? 1 : 0;

2019/10/16
CSL\CMSIS\DeviceSupport\system_SWM190.c 文件中 SYS_PLL_SRC 可取值添加 SYS_CLK_48MHz

2019/10/18
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，ADC_TypeDef 定义中将 TRIG 重命名为 TRGMSK

2019/11/22
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，SYS_TypeDef 定义中添加 PRSTEN、PRSTR1、PRSTR2三个寄存器定义

2019/12/09
CSL\SWM190_StdPeriph_Driver\SWM190_cordic.c 文件中，添加 CORDIC_FullCycleSin()、CORDIC_FullCycleSin_Angle() 等四个函数定义

2019/12/17
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，将：
#define SFC_CFG_CLKDIV_Msk			(0x01 << SFC_CFG_CLKDIV_Pos)
修正为：
#define SFC_CFG_CLKDIV_Msk			(0x03 << SFC_CFG_CLKDIV_Pos)

2019/12/25
CSL\CMSIS\DeviceSupport\system_SWM190.c 文件中 SystemCoreClockUpdate() 函数中，将：
if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)			//PLL_IN <= HRC
{
	SystemCoreClock = __HSI;
}
修正为：
if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)			//PLL_IN <= HRC
{
	if(SYS->HRCCR & SYS_HRCCR_DBL_Msk)				//HRC = 48MHz
	{
		SystemCoreClock = __HSI*2;
	}
	else											//HRC = 24MHz
	{
		SystemCoreClock = __HSI;
	}
}

2020/03/03
CSL\SWM190_StdPeriph_Driver\SWM190_i2c.c 文件中，添加 I2C_Start、I2C_Stop、I2C_Write、I2C_Read 函数定义

2020/05/12
CSL\SWM190_StdPeriph_Driver\SWM190_flash.c 文件中，FLASH_Erase、FLASH_Write 中执行操作前先关闭全局中断

2020/06/22
CSL\CMSIS\DeviceSupport\system_SWM190.c 文件中，将：
SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (7 << SYS_XTALCR_DRV_Pos);
修改为：
SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (7 << SYS_XTALCR_DRV_Pos) | (1 << SYS_XTALCR_DET_Pos);
即开启停振检测，32k晶振也做类似处理

2020/08/19
CSL\SWM190_StdPeriph_Driver\SWM190_wdt.c 文件中，WDT_Init()中将：
if(rst_period == 0)
{
	WDTx->CR &= ~(1 << WDT_CR_RSTEN_Pos);
}
修正为：
if(rst_period == 0)
{
	WDTx->CR &= ~(1 << WDT_CR_RSTEN_Pos);
	
	WDTx->RSTVAL = int_period;
}

2020/10/21
CSL\SWM190_StdPeriph_Driver\SWM190_spi.c 文件中，SPI_Init()中添加对2分频和4分频的支持

2020/12/25
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，优化 PORT_TypeDef 定义，从而简化 PORT_Init()、GPIO_Init() 函数定义

CSL\SWM190_StdPeriph_Driver\SWM190_timr.c 文件中，优化 TIMR_Init() 定义，将预分频从period中分离出来，提高易用性

2021/05/17
CSL\SWM190_StdPeriph_Driver\SWM190_adc.c 文件中，ADC_Init() 中添加如下代码：
	if(initStruct->trig_src > 0x100)
	{
		ADCx->TRGMSK = 0xFF;
		ADCx->TRGMSK &= ~(initStruct->trig_src & 0xFF);
		
		initStruct->trig_src = initStruct->trig_src >> 8;
	}
从而使得ADC0、ADC1可以使用不同的PWM通道触发

2021/5/21
CSL\CMSIS\DeviceSupport\SWM190.h 文件中，SYS_TypeDef 定义中，将PRSTR1、PRSTR2重命名为PRSTR0、PRSTR1

CSL\SWM190_StdPeriph_Driver\SWM190_flash.c 文件中，FLASH_Init()中，将：
	SFC->TIM &= ~(SFC_TIM_WIP_CHK_ITV_Msk | SFC_TIM_WIP_CHK_LMT_Msk);
	SFC->TIM |= (0xFF << SFC_TIM_WIP_CHK_ITV_Pos) |
				(0xFF << SFC_TIM_WIP_CHK_LMT_Pos);
修改为：
	SFC->TIM &= ~(SFC_TIM_WIP_CHK_ITV_Msk | SFC_TIM_WIP_CHK_LMT_Msk);
	SFC->TIM |= (48 << SFC_TIM_WIP_CHK_ITV_Pos) |	// (2000 * 48) / 48MHz = 2ms
				(0xFF << SFC_TIM_WIP_CHK_LMT_Pos);	// 2ms * 255 = 510ms
解决页写时间过长问题

2021/6/17
CSL\SWM190_StdPeriph_Driver\SWM190_timr.c 文件中，IM固定为0、IE开关中断，改为：IE固定为0x1F、IM开关中断
因为IE为0时IF无法置位，因此无法使用轮询方式查询中断标志

注意：BTIMR没有IM寄存器，因此只能通过NVIC_EnableIRQ()、NVIC_DisableIRQ开关BTIMR中断。
BTIMR1的中断向量SPI1_BTIMR1_Handler与SPI1共用，因此开关SPI1中断时会影响到BTIMR1中断。

2021/08/10
CSL\SWM190_StdPeriph_Driver\SWM190_rtc.c 文件中
RTC_Init() 函数中
RTCx->YEAR = initStruct->Year - 1901;
修正为：
RTCx->YEAR = initStruct->Year;

RTC_GetDateTime() 函数中
dateTime->Year = RTCx->YEAR + 1901;
修正为：
dateTime->Year = RTCx->YEAR;

2022/09/30
CSL\CMSIS\DeviceSupport\system_SWM190.c 文件中，SystemInit() 函数中，添加如下内容（即 FLASH_Init() 函数内容）：
	SFC->TIM &= ~(SFC_TIM_WIP_CHK_ITV_Msk | SFC_TIM_WIP_CHK_LMT_Msk);
	SFC->TIM |= (48 << SFC_TIM_WIP_CHK_ITV_Pos) |	// (2000 * 48) / 48MHz = 2ms
				(0xFF << SFC_TIM_WIP_CHK_LMT_Pos);	// 2ms * 255 = 510ms
并删除 FLASH_Init() 定义，防止用户忘记调用 FLASH_Init() 导致 Flash 写操作失败

2022/10/20
CSL\SWM190_StdPeriph_Driver\SWM190_uart.c 文件中，将：
void UART_INTRXThresholdEn(UART_TypeDef * UARTx);
void UART_INTRXThresholdDis(UART_TypeDef * UARTx);
uint32_t UART_INTRXThresholdStat(UART_TypeDef * UARTx);
void UART_INTTXThresholdEn(UART_TypeDef * UARTx);
void UART_INTTXThresholdDis(UART_TypeDef * UARTx);
uint32_t UART_INTTXThresholdStat(UART_TypeDef * UARTx);
void UART_INTTimeoutEn(UART_TypeDef * UARTx);
void UART_INTTimeoutDis(UART_TypeDef * UARTx);
uint32_t UART_INTTimeoutStat(UART_TypeDef * UARTx);
void UART_INTTXDoneEn(UART_TypeDef * UARTx);
void UART_INTTXDoneDis(UART_TypeDef * UARTx);
uint32_t UART_INTTXDoneStat(UART_TypeDef * UARTx);
简化为：
void UART_INTEn(UART_TypeDef * UARTx, uint32_t it);
void UART_INTDis(UART_TypeDef * UARTx, uint32_t it);
void UART_INTClr(UART_TypeDef * UARTx, uint32_t it);
uint32_t UART_INTStat(UART_TypeDef * UARTx, uint32_t it);

CSL\SWM190_StdPeriph_Driver\SWM190_uart.c 文件中，UART_Init() 函数中，将：
	UARTx->TOCR |= (initStruct->TimeoutTime << UART_TOCR_TIME_Pos);
修改为：
	UARTx->TOCR |= (1 << UART_TOCR_MODE_Pos) |
				   (initStruct->TimeoutTime << UART_TOCR_TIME_Pos);

修改 CSL\SWM190_StdPeriph_Driver\SWM190_dma.c 和 SWM190_dma.h 文件，使用 Scatter-Gather 模式实现 ping-pong 功能

2022/10/21
CSL\SWM190_StdPeriph_Driver\SWM190_spi.c 文件中，将：
void SPI_INTRXHalfFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXHalfFullDis(SPI_TypeDef * SPIx);
void SPI_INTRXHalfFullClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXHalfFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXFullDis(SPI_TypeDef * SPIx);
void SPI_INTRXFullClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowEn(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowDis(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXOverflowStat(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyEn(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyDis(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXEmptyStat(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteEn(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteDis(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXCompleteStat(SPI_TypeDef * SPIx);
void SPI_INTRXThresholdEn(SPI_TypeDef * SPIx);
void SPI_INTRXThresholdDis(SPI_TypeDef * SPIx);
void SPI_INTRXThresholdClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXThresholdStat(SPI_TypeDef * SPIx);
void SPI_INTTXThresholdEn(SPI_TypeDef * SPIx);
void SPI_INTTXThresholdDis(SPI_TypeDef * SPIx);
void SPI_INTTXThresholdClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXThresholdStat(SPI_TypeDef * SPIx);
简化为：
void SPI_INTEn(SPI_TypeDef * SPIx, uint32_t it);
void SPI_INTDis(SPI_TypeDef * SPIx, uint32_t it);
void SPI_INTClr(SPI_TypeDef * SPIx, uint32_t it);
uint32_t SPI_INTStat(SPI_TypeDef * SPIx, uint32_t it);
