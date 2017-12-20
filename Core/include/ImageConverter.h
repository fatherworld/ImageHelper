#ifndef _Image_Converter_h_
#define _Image_Converter_h_

#include "Format.h"

#ifdef AlvaPublic
#include "ImageHelper_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	int ImageTypeConverter(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt iFmt, alvafmt oFmt);

#ifdef __cplusplus
}
#endif

#endif