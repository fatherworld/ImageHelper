#ifndef _string_fl_h_
#define _string_fl_h_

#include "Config.h"

#include <string.h>
#include <wchar.h>

#if OS_ALVA == OS_Windows
#define strcpy_fl(dst, count, src)   strcpy_s(dst, count, src)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strcpy_fl(dst, count, src)   strcpy(dst, src)
#endif

#if OS_ALVA == OS_Windows
#define strncpy_fl(dst, dstsize, src, count)   strncpy_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strncpy_fl(dst, dstsize, src, count)   strncpy(dst, src, count)
#endif

#if OS_ALVA == OS_Windows
#define strcat_fl(dst, count, src)   strcat_s(dst, count,src)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strcat_fl(dst, count, src)   strcat(dst, src)
#endif

#if OS_ALVA == OS_Windows
#define strncat_fl(dst, dstsize, src, count)   strncat_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strncat_fl(dst, dstsize, src, count)   strncat(dst, src, count)
#endif

#if OS_ALVA == OS_Windows
#define memmove_fl(dst, dstsize, src, count)   memmove_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define memmove_fl(dst, dstsize, src, count)   memmove(dst, src, count)
#endif

#if OS_ALVA == OS_Windows
#define strtok_fl(str, delim, context)   strtok_s(str, delim, context)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strtok_fl(str, delim, context)   strtok(str, delim)
#endif

#if OS_ALVA == OS_Windows
#define wcstok_fl(wstr, wdelim, context)  wcstok_s(wstr, wdelim, context)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wcstok_fl(wstr, wdelim, context)  wcstok(wstr, wdelim)
#endif

#if OS_ALVA == OS_Windows
#define wcsncat_fl(dst, dstsize, src, count)   wcsncat_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wcsncat_fl(dst, dstsize, src, count)   wcsncat(dst, src, count)
#endif

#if OS_ALVA == OS_Windows
#define wcscpy_fl(dst, count, src)   wcscpy_s(dst,count,src)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wcscpy_fl(dst, count, src)   wcscpy(dst, src)
#endif

#if OS_ALVA == OS_Windows
#define wcsncpy_fl(dst, dstsize, src, count)   wcsncpy_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wcsncpy_fl(dst, dstsize, src, count)   wcsncpy(dst, src, count)
#endif

#if OS_ALVA == OS_Windows
#define strupr_fl(src)   _strupr_s(src)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define strupr_fl(szMsg)\
{\
    char *pcMsg = NULL;\
    for (pcMsg = szMsg; ('\0' != *pcMsg); pcMsg++){\
        if (('a' <= *pcMsg) && (*pcMsg <= 'z')){\
            *pcMsg -= ('a' - 'A');\
        }\
    }\
}
#endif

#endif