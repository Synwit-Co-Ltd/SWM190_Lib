#ifndef __SWM190_SLEEP_H__
#define __SWM190_SLEEP_H__


#if defined ( __ICCARM__ )

__ramfunc void EnterSleepMode(void);
__ramfunc void EnterStopMode(void);

#else

void EnterSleepMode(void);
void EnterStopMode(void);

#endif


#endif //__SWM190_SLEEP_H__
