#ifndef _ALIGNED_MALLOC_H_
#define _ALIGNED_MALLOC_H_

#ifdef AlvaPublic
#include "std_multi_platform_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    void* aligned_malloc(size_t size, int alignment);

    void aligned_free(void* aligned);

    int isAligned(void* data, int alignment);
#ifdef __cplusplus
}
#endif
#endif