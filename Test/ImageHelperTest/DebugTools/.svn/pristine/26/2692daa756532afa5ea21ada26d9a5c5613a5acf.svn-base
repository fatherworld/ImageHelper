#include "Config.h"

#if OS_ALVA == OS_Windows
#include <Windows.h>
#include <time.h>
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_Linux || OS_ALVA == OS_iOS || OS_ALVA == OS_MacOS
#include <sys/time.h>
#endif

#include "AlvaTimer.h"

#undef FILE_NUM
#define FILE_NUM 0x8100

#define TIME_us 1.0f
#define TIME_ms 1000.0f
#define TIME_s  1000000.0f

#if OS_ALVA == OS_Windows
static int gettimeofday(struct timeval* tp, void* tzp){

#undef FUNC_CODE
#define FUNC_CODE 0x01

    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;

    return 0;
}
#endif

void startTime(Timer* timer){

#undef FUNC_CODE
#define FUNC_CODE 0x02

    struct timeval cs;
    gettimeofday(&cs, NULL);
    timer->start = cs.tv_sec * 1000000 + cs.tv_usec;
}

void stopTime(Timer* timer){

#undef FUNC_CODE
#define FUNC_CODE 0x03

    struct timeval ce;
    gettimeofday(&ce, NULL);
    timer->end = ce.tv_sec * 1000000 + ce.tv_usec;
}

long diffTime(Timer timer){

#undef FUNC_CODE
#define FUNC_CODE 0x04

    return (long)(timer.end - timer.start);
}

long long Time() {

#undef FUNC_CODE
#define FUNC_CODE 0x05

    struct timeval cs;
    gettimeofday(&cs, NULL);
    return cs.tv_sec;
}