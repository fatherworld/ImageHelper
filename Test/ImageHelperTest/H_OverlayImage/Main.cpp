#include "Config.h"

#include "malloc.h"

#include "stdio_fl.h"
#include "string_fl.h"

#include "Format.h"

#define LOG_TAG "Main"
#include "Log.h"

#include "Dump.h"
#include "ErrorTools.h"

#include "EulerRotationMatrix.h"

#include "AndroidHelper.h"

#include "BmpHelper.h"
#include "ImageConverter.h"
#include "ImageRotate.h"

int main(int argc, char** argv) {

    char* srcImg = NULL, *dstImg = NULL, *hPath = NULL;

    int err = 0;
    if (argc == 1) {
        srcImg = (char*)malloc(2048);
        LOGE("please input source image name: \n");
        gets_fl(srcImg, 2048);
        dstImg = (char*)malloc(2048);
        LOGE("please input referance image name: \n");
        gets_fl(dstImg, 2048);
        hPath = (char*)malloc(2048);
        LOGE("please input H name: \n");
        gets_fl(hPath, 2048);
    }
    else if (argc == 2) {
        srcImg = (char*)malloc(2048);
        strcpy_fl(srcImg, 2048, argv[1]);
        dstImg = (char*)malloc(2048);
        LOGE("please input referance image name: \n");
        gets_fl(dstImg, 2048);
        hPath = (char*)malloc(2048);
        LOGE("please input H name: \n");
        gets_fl(hPath, 2048);
    }
    else if (argc == 3) {
        srcImg = (char*)malloc(2048);
        strcpy_fl(srcImg, 2048, argv[1]);
        dstImg = (char*)malloc(2048);
        strcpy_fl(dstImg, 2048, argv[2]);
        hPath = (char*)malloc(2048);
        LOGE("please input H name: \n");
        gets_fl(hPath, 2048);
    }
    else if (argc == 4) {
        srcImg = (char*)malloc(2048);
        strcpy_fl(srcImg, 2048, argv[1]);
        dstImg = (char*)malloc(2048);
        strcpy_fl(dstImg, 2048, argv[2]);
        hPath = (char*)malloc(2048);
        strcpy_fl(hPath, 2048, argv[3]);
    }
    else {
        LOGE("parameter error, use: xxx.exe srcImg referanceImg hfile\n");
        return -1;
    }

    BMP bmpSrc, bmpRef;
    LoadBMP(srcImg, &bmpSrc, BIT32);
    LoadBMP(dstImg, &bmpRef, BIT32);

    unsigned char* graySrc = NULL;
    unsigned char* hLCGrayForwardRef = NULL, *hLCGrayBackwardRef = NULL;
    unsigned char* hCTGrayForwardRef = NULL, *hCTGrayBackwardRef = NULL;

    ImageTypeConverter(bmpSrc.data, &graySrc, bmpSrc.width, bmpSrc.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);
    ImageTypeConverter(bmpRef.data, &hLCGrayForwardRef, bmpRef.width, bmpRef.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);
    ImageTypeConverter(bmpRef.data, &hLCGrayBackwardRef, bmpRef.width, bmpRef.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);
    ImageTypeConverter(bmpRef.data, &hCTGrayForwardRef, bmpRef.width, bmpRef.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);
    ImageTypeConverter(bmpRef.data, &hCTGrayBackwardRef, bmpRef.width, bmpRef.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);

    float H[9];
    char* packageData = NULL;
    int length = 0;
    char* ptr = NULL;

    err = readData(hPath, Alva_File, &packageData, &length);
    if (ERROR_CHECK(err))
        return getchar();
    ptr = packageData;

    char * p = ptr;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            p = strstr(ptr, ",");
            *p = '\0';
            sscanf_fl(ptr, "%f", &(H[i * 3 + j]));
            ptr = p + 1;
        }
        p = strstr(ptr, "\n");
        *p = '\0';

        sscanf_fl(ptr, "%f", &(H[i * 3 + 2]));
        ptr = p + 1;
    }

    LOGE("H:\n");
    for (int i = 0; i < 3; i++) {
        LOGE("%9.7f %9.7f %9.7f\n", H[i * 3 + 0], H[i * 3 + 1], H[i * 3 + 2]);
    }

    int oW = bmpRef.width, oH = bmpRef.height;
    H_OverlayImageForward(H, graySrc, bmpSrc.width, bmpSrc.height, hLCGrayForwardRef, oW, oH, CT_LeftUpCorner);
    H_OverlayImageBackward(H, graySrc, bmpSrc.width, bmpSrc.height, hLCGrayBackwardRef, oW, oH, CT_LeftUpCorner);

    dump_bmp("LeftUp_OverlayImage_Forward.bmp", (char*)hLCGrayForwardRef, bmpRef.width, bmpRef.height, 1, DUMP_UCHAR);
    dump_bmp("LeftUp_OverlayImage_Backward.bmp", (char*)hLCGrayBackwardRef, bmpRef.width, bmpRef.height, 1, DUMP_UCHAR);

    H_OverlayImageForward(H, graySrc, bmpSrc.width, bmpSrc.height, hCTGrayForwardRef, oW, oH, CT_ImageCenter);
    H_OverlayImageBackward(H, graySrc, bmpSrc.width, bmpSrc.height, hCTGrayBackwardRef, oW, oH, CT_ImageCenter);

    dump_bmp("Center_OverlayImage_Forward.bmp", (char*)hCTGrayForwardRef, bmpRef.width, bmpRef.height, 1, DUMP_UCHAR);
    dump_bmp("Center_OverlayImage_Backward.bmp", (char*)hCTGrayBackwardRef, bmpRef.width, bmpRef.height, 1, DUMP_UCHAR);

    return 0;
}