#ifndef _TIMER_H_FL
#define _TIMER_H_FL

#ifdef AlvaPublic
#include "DebugTools_Hide.h"
#endif

typedef struct _Timer_{
    long long start;
    long long end;
}Timer;

#ifdef __cplusplus
extern "C"
{
#endif
void startTime(Timer* timer);
void stopTime(Timer* timer);
long diffTime(Timer timer);

#define ShowTime(name, timer) LOGE("Time %s: %ldus\n", name, diffTime(timer));
#define ShowTimeIndex(name, index, timer) LOGE("Time %s[%d]: %ldus\n", name, index, diffTime(timer));

#if AlvaTime & AlvaTime_TotalPerf
#define TimerTotalPerf(timer) Timer timer;
#define StartTotalPerf(timer) startTime(&timer);
#define StopTotalPerf(name, timer) stopTime(&timer);ShowTime(name, timer);
#else
#define TimerTotalPerf(timer) 
#define StartTotalPerf(timer) 
#define StopTotalPerf(name, timer) 
#endif

#if AlvaTime & AlvaTime_Function
#define TimerFunction(timer) Timer timer;
#define StartFunction(timer) startTime(&timer);
#define StopFunction(name, timer) stopTime(&timer);ShowTime(name, timer);
#define StopFunctionIndex(name, index, timer) stopTime(&timer);ShowTimeIndex(name, index, timer);
#else
#define TimerFunction(timer) 
#define StartFunction(timer) 
#define StopFunction(name, timer) 
#define StopFunctionIndex(name, index, timer) 
#endif

//precise time in second of now(since 1970.1.1)
long long Time(void);
#ifdef __cplusplus
}
#endif

#endif
