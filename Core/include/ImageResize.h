#ifndef _Image_Resize_h_
#define _Image_Resize_h_

#ifdef __cplusplus
extern "C"
{
#endif

    int ImageResize(unsigned char* igray, unsigned char* ogray, int iwidth, int iheight, int owidth, int oheight);

#ifdef __cplusplus
}
#endif

#endif