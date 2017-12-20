#include "Config.h"

#include "stdlib_fl.h"

#include "aligned_malloc.h"

void* aligned_malloc(size_t size, int alignment)
{
    // 分配足够的内存, 这里的算法很经典, 早期的STL中使用的就是这个算法    

    // 首先是维护FreeBlock指针占用的内存大小    
    const int pointerSize = sizeof(void*);

    // alignment - 1 + pointerSize这个是FreeBlock内存对齐需要的内存大小    
    // 前面的例子sizeof(T) = 20, __alignof(T) = 16,    
    // g_MaxNumberOfObjectsInPool = 1000    
    // 那么调用本函数就是alignedMalloc(1000 * 20, 16)    
    // 那么alignment - 1 + pointSize = 19    
    const size_t requestedSize = size + alignment - 1 + pointerSize;

    // 分配的实际大小就是20000 + 19 = 20019    
    void* raw = malloc(requestedSize);

    // 这里实Pool真正为对象实例分配的内存地址    
    uintptr_t start = (uintptr_t)raw + pointerSize;
    // 向上舍入操作    
    // 解释一下, __ALIGN - 1指明的是实际内存对齐的粒度    
    // 例如__ALIGN = 8时, 我们只需要7就可以实际表示8个数(0~7)    
    // 那么~(__ALIGN - 1)就是进行舍入的粒度    
    // 我们将(bytes) + __ALIGN-1)就是先进行进位, 然后截断    
    // 这就保证了我是向上舍入的    
    // 例如byte = 100, __ALIGN = 8的情况    
    // ~(__ALIGN - 1) = (1 000)B    
    // ((bytes) + __ALIGN-1) = (1 101 011)B    
    // (((bytes) + __ALIGN-1) & ~(__ALIGN - 1)) = (1 101 000 )B = (104)D    
    // 104 / 8 = 13, 这就实现了向上舍入    
    // 对于byte刚好满足内存对齐的情况下, 结果保持byte大小不变    
    // 记得《Hacker's Delight》上面有相关的计算    
    // 这个表达式与下面给出的等价    
    // ((((bytes) + _ALIGN - 1) * _ALIGN) / _ALIGN)    
    // 但是SGI STL使用的方法效率非常高     
    void* aligned = (void*)((start + alignment - 1) & ~(alignment - 1));

    // 这里维护一个指向malloc()真正分配的内存    
    *(void**)((uintptr_t)aligned - pointerSize) = raw;

    // 返回实例对象真正的地址    
    return aligned;
}


// 这里是内部维护的内存情况    
//                   这里满足内存对齐要求    
//                             |    
// ----------------------------------------------------------------------    
// | 内存对齐填充 | 维护的指针 | 对象1 | 对象2 | 对象3 | ...... | 对象n |    
// ----------------------------------------------------------------------    
// ^                     | 指向malloc()分配的地址起点    
// |                     |    
// -----------------------    
void aligned_free(void* aligned)
{
    // 释放操作很简单了, 参见上图    
    void* raw = *(void**)((uintptr_t)aligned - sizeof(void*));
    free(raw);
}

int isAligned(void* data, int alignment)
{
    // 又是一个经典算法, 参见<Hacker's Delight>    
    return ((uintptr_t)data & (alignment - 1)) == 0;
}