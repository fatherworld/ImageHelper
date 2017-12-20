#ifndef _Image_Filter_H_
#define _Image_Filter_H_

#ifdef AlvaPublic
#include "ImageHelper_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum _ImageFilter_Type_ {
        Type_Gauss                  = 1,
        Type_Bright                 = 2,
    }ImageFilter_Type;

    typedef float* GaussParam;

    void genFilter(float* filter, int radiusX, int radiusY, ImageFilter_Type type, void* param);

    void ApplyFilter(unsigned char* image, int width, int height, unsigned char* oImage, float* filter, int radiusX, int radiusY);

#ifdef __cplusplus
}
#endif
#endif