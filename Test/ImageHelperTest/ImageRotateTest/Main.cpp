#include "Config.h"

#include "malloc.h"

#include "stdio_fl.h"

#define LOG_TAG "Main"
#include "Log.h"
#include "Dump.h"

#include "EulerRotationMatrix.h"

#include "BmpHelper.h"
#include "ImageConverter.h"
#include "ImageRotate.h"

int main(int argc, char** argv) {

    char* srcImg = NULL;

    int err = 0;
    if (argc == 1) {
        srcImg = (char*)malloc(2048);
        LOGE("please input image name: \n");
        gets_fl(srcImg, 2048);
    }

    BMP bmp;
    LoadBMP(srcImg, &bmp, BIT32);

    unsigned char* gray = NULL;

    ImageTypeConverter(bmp.data, &gray, bmp.width, bmp.height, Alva_FMT_BGRA_8888, Alva_FMT_GRAY);

    int ow = 0, oh = 0, os = 0;
    unsigned char* oi = NULL;
    float H[9] = { 0.0f };
    int omin[2] = { 0 }, omax[2] = { 0 }, owh[2] = { 0 };
    float center[2] = { bmp.width / 2.0f, bmp.height / 2.0f };

    float f = 1303.0f, s = 0.1f;

    for (float tx = 0.0f; tx < 1.0f; tx += 1.0f) {
        for (float ty = 0.0f; ty < 1.0f; ty += 1.0f) {
            for (float tz = 0.0f; tz < 0.5f; tz += 0.5f) {
                for (float rx = -20.0; rx < 21.0f; rx += 10.0f) {
                    for (float ry = -20.0f; ry < 21.0f; ry += 10.0f) {
                        for (float rz = 0.0f; rz < 360.0f; rz += 60.0f) {
                            rt2H(Angle2Radian(rx), Angle2Radian(ry), Angle2Radian(rz), tx, ty, tx, f, s, H);

                            computeHOutRect(bmp.width, bmp.height, H, center, omin, omax, owh);

                            oi = (unsigned char*)malloc(owh[0] * owh[1]);
                            os = 0;
                            rotateImageH(gray, bmp.width, bmp.height, oi, &ow, &oh, &os, Angle2Radian(rx), Angle2Radian(ry), Angle2Radian(rz), tx, ty, tx, f, s);

                            char name[2048] = { 0 };
                            sprintf_fl(name, "Dump\\RI_[%f_%f_%f]_[%f_%f_%f].bmp", rx, ry, rz, tx, ty, tx);
                            DUMP_BMP(name, (char*)oi, ow, oh, 1, DUMP_UCHAR);

                            sprintf_fl(name, "Dump\\H_[%f_%f_%f]_[%f_%f_%f].txt", rx, ry, rz, tx, ty, tx);
                            void* file = Start_FileDump(name, DUMP_APPEND);
                            LOG2File(file, "%8.6f, %8.6f, %8.6f\n", H[0], H[1], H[2]);
                            LOG2File(file, "%8.6f, %8.6f, %8.6f\n", H[3], H[4], H[5]);
                            LOG2File(file, "%8.6f, %8.6f, %8.6f\n", H[6], H[7], H[8]);
                            End_FileDump(file);
                            free(oi);
                        }
                    }
                }
            }
        }
    }
    return 0;
}