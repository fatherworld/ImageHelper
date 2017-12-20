#ifndef _memory_fl_h_
#define _memory_fl_h_

#include "Config.h"

#include <string.h>

#if OS_ALVA == OS_Windows
#define memcpy_fl(dst, dstsize, src, count)   memcpy_s(dst, dstsize, src, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define memcpy_fl(dst, dstsize, src, count)   memcpy(dst, src, count)
#endif

#endif
