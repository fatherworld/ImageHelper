#include "Config.h"

#include "stdio_fl.h"
#include "memory_fl.h"

#define LOG_TAG "ImageDistortionCorrection"
#include "Log.h"
#include "ErrorTools.h"

#include "ImageDistortionCorrection.h"

#undef FILE_NUM
#define FILE_NUM 0xF500

#define DistortionCorrectionClamp(v, minV, maxV) ((v) > (maxV) ? (maxV) : ((v) < (minV) ? (minV) : (v)))

static void H_ImageDistortionCorrection(unsigned char* sGray, unsigned char* tGray, int width, int height, float* HInv)
{
    float temp;
    float x, y;
    int i, j;
    int xi, yi;
    float threa = 1.4f;
    int tWidth = (int)(width / threa);
    memset(tGray, 0, sizeof(unsigned char) * width * height);
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < tWidth; j++)
        {
            y = i * threa;
            x = j * threa;
            temp = HInv[6] * x + HInv[7] * y + 1.0f;
            xi = (int)((HInv[0] * x + HInv[1] * y + HInv[2]) / temp);
            yi = (int)((HInv[3] * x + HInv[4] * y + HInv[5]) / temp);
            xi = width - DistortionCorrectionClamp(xi, 0, (width - 1));
            if (xi >= 0 && xi < width && yi >= 0 && yi < height)
                tGray[i * width + j] = sGray[yi * width + xi];
            else
                tGray[i * width + j] = (unsigned char)0;
        }
    }
    memcpy_fl(sGray, sizeof(unsigned char) * width * height, tGray, sizeof(unsigned char) * width * height);
}

void ImageDistortionCorrection(unsigned char* inGray, int inWidth, int inHeight, unsigned char* outGray, int outWidth, int outHeight, float* DCData, DCType dcType) {

    switch (dcType)
    {
    case DC_H:
        H_ImageDistortionCorrection(inGray, outGray, inWidth, inHeight, DCData);
        break;
    default:
        break;
    }
}