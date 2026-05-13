#ifndef __SWM2X1_SLEEP_H__
#define __SWM2X1_SLEEP_H__


#if defined ( __ICCARM__ )

__ramfunc void EnterSleepMode(void);
__ramfunc void EnterStopMode(void);

#else

void EnterSleepMode(void);
void EnterStopMode(void);

#endif


#endif //__SWM2X1_SLEEP_H__
