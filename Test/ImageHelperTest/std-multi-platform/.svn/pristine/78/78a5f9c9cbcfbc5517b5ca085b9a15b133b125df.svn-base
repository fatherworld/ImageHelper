#ifndef _stdlib_fl_h_
#define _stdlib_fl_h_

#include "Config.h"

#include <stdlib.h>

#if OS_ALVA == OS_Windows
#ifndef __cplusplus
//max, min函数定义在stdlib.h中
#endif//__cplusplus
#endif//OS_Windows
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#ifndef __cplusplus
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif//__cplusplus
#endif//OS_Android || OS_iOS

#endif