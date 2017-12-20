#ifndef _BMP_HELPER_H_
#define _BMP_HELPER_H_

#ifdef AlvaPublic
#include "ImageHelper_Hide.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum _BIT_DATA_TYPE_ {
        BIT32 = 1,                      //读取或存储成32位
        BIT24 = 2,                      //读取或存储成24位
    }BITDATATYPE;

    typedef struct _BMP_{
        int width;
        int height;
        int channels;
        unsigned char * data;
    }BMP;

    int LoadBMP(const char * file, BMP* out_img, BITDATATYPE bit_data_type);
    int WriteBMP(const char * file, BMP * in_img, BITDATATYPE bit_data_type);
    int FreeBMP(BMP* img);

#ifdef __cplusplus
}
#endif

#endif
