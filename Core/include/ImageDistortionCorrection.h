/*
 * 图片畸变矫正
 */
#ifndef _ImageDistortionCorrection_h_
#define _ImageDistortionCorrection_h_

typedef enum _DistortionCorrectionType_ {
    DC_Start = -1,
    DC_H,
}DCType;

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * ImageDistortionCorrection 对uChar灰度图的畸变矫正
     * @inGray : 输入的灰度图
     * @inWidth: 输入灰度图的宽
     * @inHeight: 输入灰度图的高
     * @outGray: 输出的灰度图
     * @outWidth: 输出灰度图的宽
     * @outHeight: 输出灰度图的高
     * @DCData : 畸形矫正需要用到的参数
     * @DCType: 畸形矫正的模式
     * return: null
     * 注: 输出图的内存空间需要使用者开辟  释放
     */
    void ImageDistortionCorrection(unsigned char* inGray, int inWidth, int inHeight, unsigned char* outGray, int outWidth, int outHeight, float* DCData, DCType dcType);

#ifdef __cplusplus
}
#endif

#endif
