#include "Config.h"

#include <stdlib.h>
#include "stdio_fl.h"
#include "memory_fl.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define LOG_TAG "ImageFilter"
#include "Log.h"
#include "ErrorTools.h"

#include "ImageFilter.h"

#undef FILE_NUM
#define FILE_NUM 0xF400

void ApplyFilter(unsigned char* image, int width, int height, unsigned char* oImage, float* filter, int radiusX, int radiusY) {

#undef FUNC_CODE
#define FUNC_CODE 0x01

    int x = 0, y = 0, offx = 0, offy = 0, startY = 0, startX = 0, startOffY = 0;
    float value = 0;

    int filterI = 0;

    for (y = 0; y < radiusY; y++) {
        startY = y * width;
        for (x = 0; x < width; x++) {
            startX = startY + x;
            oImage[startX] = 0;
        }
    }

    for (y = radiusY; y < height - radiusY; y++) {
        startY = y * width;
        for (x = 0; x < radiusX; x++) {
            startX = startY + x;
            oImage[startX] = 0;
        }
        for (x = radiusX; x < width - radiusX; x++) {
            startX = startY + x;
            value = 0.0f;
            filterI = 0;
            startOffY = -radiusY * width;
            for (offy = -radiusY; offy <= radiusY; offy++) {
                for (offx = -radiusX; offx <= radiusX; offx++) {
                    value += image[startX + startOffY + offx] * filter[filterI];
                    filterI++;
                }
                startOffY += width;
            }
            oImage[startX] = (unsigned char)value;
        }
        for (x = width - radiusX; x < width; x++) {
            startX = startY + x;
            oImage[startX] = 0;
        }
    }

    for (y = height - radiusY; y < height; y++) {
        startY = y * width;
        for (x = 0; x < width; x++) {
            startX = startY + x;
            oImage[startX] = 0;
        }
    }
}

void innerGenGauss(float* filter, int radiusX, int radiusY, float* param) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    int i, j;
    int filterW = radiusX * 2 + 1;

    float sigma2 = *param * *param;
    float coef = (float)(1.0f / (2 * M_PI * sigma2));
    float sum = 0.0f, temp;
    for (j = -radiusY; j <= radiusY; j++) {
        for (i = -radiusX; i <= radiusX; i++) {
            temp = (float)(coef * exp(-(i * i + j * j) / (2 * sigma2)));
            sum += temp;
            filter[(j + radiusY) * filterW + (i + radiusX)] = temp;
        }
    }

    filterW = filterW * (2 * radiusY + 1);
    for (i = 0; i < filterW; i++) {
        filter[i] /= sum;
    }
}

void innerGenBright(float* filter, int radiusX, int radiusY, float* param) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    filter[0] = param[0];
}

void genFilter(float* filter, int radiusX, int radiusY, ImageFilter_Type type, void* param) {

#undef FUNC_CODE
#define FUNC_CODE 0x03

    switch (type) {
        case Type_Gauss:
            innerGenGauss(filter, radiusX, radiusY, (float*)param);
            break;
        case Type_Bright:
            innerGenBright(filter, radiusX, radiusY, (float*)param);
            break;
        default:
            break;
    }
}
