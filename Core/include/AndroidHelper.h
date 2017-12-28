#ifndef _ANDROID_HELPER_H_
#define _ANDROID_HELPER_H_

#ifdef AlvaPublic
#include "JniHelper_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    int Android_GetPackageName(char* outPackageName, int length);

#if OS_ALVA == OS_Android
    char* Jstring2CStr(jstring jstr);
#endif

    int readData(const char *filename, alvaft type, char **sources, int *fileLengths);

#ifdef __cplusplus
}
#endif

#endif