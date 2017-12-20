#include "Config.h"

#include "stdio_fl.h"
#include <math.h>
#include "memory_fl.h"
#include "string_fl.h"

#define LOG_TAG "ImageRotate"
#include "Log.h"
#include "ErrorTools.h"

#include "EulerRotationMatrix.h"

#include "ImageRotate.h"

#undef FILE_NUM
#define FILE_NUM 0xF200

/*
* 采用经纬度旋转
* sita: 纬度
* fai: 经度
* R_sita = [cos_sita   -sin_sita]
*          [sin_sita    cos_sita]
*
* R_fai  = [cos_fai    -sin_fai ]
*          [sin_fai     cos_fai ]
*
* coord  = [x               y   ]'
*
* [x', y'] = R_fai * coord * R_sita
*/

/*
* 采用经纬度旋转反向计算坐标
* sita: 纬度
* fai: 经度
* ^: 逆矩阵
* R_sita^ = [ cos_sita    sin_sita]
*           [-sin_sita    cos_sita]
*
* R_fai^  = [ cos_fai     sin_fai ]
*           [-sin_fai     cos_fai ]
*
* dstCoord= [x'               y'  ]'
*
* [x, y] = R_fai^ * dstCoord * R_sita^
*/


/*
* 两个方阵相乘
* result = m * m
* dim:方阵维度
*/
int matrix_matrix(float* a, float* b, float* result, int dim) {

#undef FUNC_CODE
#define FUNC_CODE 0x01

    int i = 0, j = 0, k = 0;

    memset(result, 0, sizeof(float) * dim * dim);

    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            for (k = 0; k < dim; k++) {
                result[i * dim + j] += a[i * dim + k] * b[k * dim + j];
            }
        }
    }
    return 0;
}

/*
* 矢量乘方阵
* vresult = v * m
* dim: 方阵维度
*/
int vector_matrix(float* v, float* m, float* vresult, int dim) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    int i = 0, k = 0;

    memset(vresult, 0, sizeof(float) * dim);

    for (i = 0; i < dim; i++) {
        for (k = 0; k < dim; k++) {
            vresult[i] += v[k] * m[k * dim + i];
        }
    }
    return 0;
}

/*
* 方阵乘矢量
* vresult = m * v
* dim: 方阵维度
*/
int matrix_vector(float* m, float* v, float* vresult, int dim) {

#undef FUNC_CODE
#define FUNC_CODE 0x03

    int i = 0, k = 0;

    memset(vresult, 0, sizeof(float) * dim);

    for (i = 0; i < dim; i++) {
        for (k = 0; k < dim; k++) {
            vresult[i] += m[i * dim + k] * v[k];
        }
    }

    return 0;
}

int getMaxBufferSize(int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x04

    return width * 10 * height * 10;
}

void ComputeRotateMatrix(float sita, float* m_sita) {

#undef FUNC_CODE
#define FUNC_CODE 0x05

    float sin_s, cos_s;

    sin_s = (float)sin(Angle2Radian(sita));
    cos_s = (float)cos(Angle2Radian(sita));

    m_sita[0] = cos_s;
    m_sita[1] = -sin_s;
    m_sita[2] = sin_s;
    m_sita[3] = cos_s;
}

void ComputeInverseRotateMatrix(float sita, float* m_sita) {

#undef FUNC_CODE
#define FUNC_CODE 0x06

    float sin_s, cos_s;

    sin_s = (float)sin(Angle2Radian(sita));
    cos_s = (float)cos(Angle2Radian(sita));

    m_sita[0] = cos_s;
    m_sita[1] = sin_s;
    m_sita[2] = -sin_s;
    m_sita[3] = cos_s;
}

int computeAffineMatrix(float sita, float fai, float lambd1, float lambd2, float* A) {

#undef FUNC_CODE
#define FUNC_CODE 0x07

    float Res_1[4], Res_2[4];

    float R_sita[4], R_fai[4];
    float IR_fai[4];

    float S[4] = { lambd1, 0.0f, 0.0f, lambd2 };

    ComputeRotateMatrix(sita, R_sita);
    ComputeRotateMatrix(fai, R_fai);

    ComputeInverseRotateMatrix(fai, IR_fai);

    matrix_matrix(R_sita, IR_fai, Res_1, 2);
    matrix_matrix(Res_1, S, Res_2, 2);
    matrix_matrix(Res_2, R_fai, A, 2);

    return 0;
}

int computeInverseAffineMatrix(float sita, float fai, float lambd1, float lambd2, float* IA) {

#undef FUNC_CODE
#define FUNC_CODE 0x08

    float Res_1[4], Res_2[4];

    float R_fai[4];
    float IR_sita[4], IR_fai[4];

    float IS[4] = { 1.0f / lambd1, 0.0f, 0.0f, 1.0f / lambd2 };

    ComputeRotateMatrix(fai, R_fai);

    ComputeInverseRotateMatrix(sita, IR_sita);
    ComputeInverseRotateMatrix(fai, IR_fai);

    matrix_matrix(IR_fai, IS, Res_1, 2);
    matrix_matrix(Res_1, R_fai, Res_2, 2);
    matrix_matrix(Res_2, IR_sita, IA, 2);

    return 0;
}

int computeOutRect(int width, int height, float sita, float fai, float lambd1, float lambd2, int* oMinCoord, int* oMaxCoord, int* wh) {

#undef FUNC_CODE
#define FUNC_CODE 0x09

    float imgCoord[4][2] = { { 0.0f, 0.0f },{ width - 1.0f, 0.0f },{ 0, height - 1.0f },{ width - 1.0f, height - 1.0f } };
    float oImgCoord[4][2] = { { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f } };

    //for check
    float cImgCoord[4][2] = { {0.0f, 0.0f}, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };

    float minCoord[2] = { 100.0f * width, 100.0f * height }, maxCoord[2] = { -100.0f * width, -100.0f * height };

    float Res_1[4], Res_2[4];
    float A[4], IA[4];

    float R_sita[4], R_fai[4];
    float IR_sita[4], IR_fai[4];
    float S[4] = { lambd1, 0.0f, 0.0f, lambd2 };
    float IS[4] = { 1.0f / lambd1, 0.0f, 0.0f, 1.0f / lambd2 };

    int i = 0;

    ComputeRotateMatrix(sita, R_sita);
    ComputeRotateMatrix(fai, R_fai);

    ComputeInverseRotateMatrix(sita, IR_sita);
    ComputeInverseRotateMatrix(fai, IR_fai);

    matrix_matrix(R_sita, IR_fai, Res_1, 2);
    matrix_matrix(Res_1, S, Res_2, 2);
    matrix_matrix(Res_2, R_fai, A, 2);

    matrix_matrix(IR_fai, IS, Res_1, 2);
    matrix_matrix(Res_1, R_fai, Res_2, 2);
    matrix_matrix(Res_2, IR_sita, IA, 2);

    for (i = 0; i < 4; i++) {
        vector_matrix(imgCoord[i], A, oImgCoord[i], 2);
    }

    for (i = 0; i < 4; i++) {
        vector_matrix(oImgCoord[i], IA, cImgCoord[i], 2);
    }

    for (i = 0; i < 4; i++) {
        if (oImgCoord[i][0] < minCoord[0])
            minCoord[0] = oImgCoord[i][0];
        if (oImgCoord[i][0] > maxCoord[0])
            maxCoord[0] = oImgCoord[i][0];

        if (oImgCoord[i][1] < minCoord[1])
            minCoord[1] = oImgCoord[i][1];
        if (oImgCoord[i][1] > maxCoord[1])
            maxCoord[1] = oImgCoord[i][1];
    }

    for (i = 0; i < 2; i++) {
        oMinCoord[i] = (int)minCoord[i];
        oMaxCoord[i] = (int)maxCoord[i];
    }

    wh[0] = (int)(maxCoord[0] - minCoord[0] + 1);
    wh[1] = (int)(maxCoord[1] - minCoord[1] + 1);

    return 0;
}

static unsigned char getImageValue(unsigned char* image, float x, float y, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0A

    int ix = (int)x;
    int iy = (int)y;

    float a, b, c, d;

    b = x - ix;
    a = 1 - b;

    d = y - iy;
    c = 1 - d;

    return (unsigned char)(image[(iy + 0) * width + (ix + 0)] * a * c +
                           image[(iy + 1) * width + (ix + 0)] * a * d +
                           image[(iy + 0) * width + (ix + 1)] * b * c +
                           image[(iy + 1) * width + (ix + 1)] * b * d);
}

int rotateImage(unsigned char* image, int width, int height, unsigned char* oImage, int* oWidth, int* oHeight, int* oStride, float sita, float fai, float lambd1, float lambd2) {

#undef FUNC_CODE
#define FUNC_CODE 0x0B

    int minCoord[2], maxCoord[2];
    int x, y;

    float coord[2];
    float Res_1[4], Res_2[4];

    float R_fai[4];
    float IR_sita[4], IR_fai[4];//逆旋转矩阵

    float IA[4];

    float IS[4] = { 1.0f / lambd1, 0.0f, 0.0f, 1.0f / lambd2 };

    int wh[2];
    computeOutRect(width, height, sita, fai, lambd1, lambd2, minCoord, maxCoord, wh);

    ComputeRotateMatrix(fai, R_fai);

    ComputeInverseRotateMatrix(sita, IR_sita);
    ComputeInverseRotateMatrix(fai, IR_fai);

    matrix_matrix(IR_fai, IS, Res_1, 2);
    matrix_matrix(Res_1, R_fai, Res_2, 2);
    matrix_matrix(Res_2, IR_sita, IA, 2);

    *oWidth = wh[0];
    *oHeight = wh[1];

    if ((*oStride) == 0) {
        *oStride = *oWidth;
    }

    for (y = 0; y < *oHeight; y++) {
        for (x = 0; x < *oWidth; x++) {
            coord[0] = (float)x + minCoord[0];
            coord[1] = (float)y + minCoord[1];

            vector_matrix(coord, IA, Res_2, 2);

            if (Res_2[0] >= 0 && Res_2[0] < width - 1 && Res_2[1] >= 0 && Res_2[1] < height - 1) {
                oImage[y * *oStride + x] = getImageValue(image, Res_2[0], Res_2[1], width, height);
            }
            else {
                oImage[y * *oStride + x] = 0;
            }
        }
    }

    return 0;
}

//单应性旋转
int HRotatePoints(float* H, float* inPoint, float* outPoint, float cx, float cy) {

#undef FUNC_CODE
#define FUNC_CODE 0x0C

    float inX = inPoint[0] - cx;
    float inY = inPoint[1] - cy;
    float temp = (H[6] * inX + H[7] * inY + H[8]);
    outPoint[0] = (H[0] * inX + H[1] * inY + H[2]) / temp + cx;
    outPoint[1] = (H[3] * inX + H[4] * inY + H[5]) / temp + cy;
    return 0;
}

int Hinverse(float* H, float* Hinverse) {

#undef FUNC_CODE
#define FUNC_CODE 0x0D

    float detH = H[0] * H[4] * H[8] +
                 H[3] * H[7] * H[2] +
                 H[6] * H[1] * H[5] -
                 H[6] * H[4] * H[2] -
                 H[7] * H[5] * H[0] -
                 H[8] * H[3] * H[1];

    float AdjH[9] = { 0.0f }; //伴随矩阵

    int i;

    if (0 == detH)
        OWN_ERROR_RETURN(0x001, "detH is 0");

    AdjH[(1 - 1) * 3 + (1 - 1)] = /*pow(-1, 1 + 1)*/ 1 * (H[4] * H[8] - H[5] * H[7]);
    AdjH[(2 - 1) * 3 + (1 - 1)] = /*pow(-1, 2 + 1)*/-1 * (H[3] * H[8] - H[5] * H[6]);
    AdjH[(3 - 1) * 3 + (1 - 1)] = /*pow(-1, 3 + 1)*/ 1 * (H[3] * H[7] - H[4] * H[6]);

    AdjH[(1 - 1) * 3 + (2 - 1)] = /*pow(-1, 1 + 2)*/-1 * (H[1] * H[8] - H[2] * H[7]);
    AdjH[(2 - 1) * 3 + (2 - 1)] = /*pow(-1, 2 + 2)*/ 1 * (H[0] * H[8] - H[2] * H[6]);
    AdjH[(3 - 1) * 3 + (2 - 1)] = /*pow(-1, 3 + 2)*/-1 * (H[0] * H[7] - H[1] * H[6]);

    AdjH[(1 - 1) * 3 + (3 - 1)] = /*pow(-1, 1 + 3)*/ 1 * (H[1] * H[5] - H[2] * H[4]);
    AdjH[(2 - 1) * 3 + (3 - 1)] = /*pow(-1, 2 + 3)*/-1 * (H[0] * H[5] - H[2] * H[3]);
    AdjH[(3 - 1) * 3 + (3 - 1)] = /*pow(-1, 3 + 3)*/ 1 * (H[0] * H[4] - H[1] * H[3]);

    for (i = 0; i < 9; i++) {
        Hinverse[i] = AdjH[i] / detH;
    }

    return 0;
}

int computeHOutRect(int width, int height, float* H, float* center, int* oMinCoord, int* oMaxCoord, int* wh) {

#undef FUNC_CODE
#define FUNC_CODE 0x0E

    float imgCoord[4][3] = { { 0.0f, 0.0f, 1.0f},{ width - 1.0f, 0.0f, 1.0f },{ 0, height - 1.0f, 1.0f },{ width - 1.0f, height - 1.0f, 1.0f } };
    float oImgCoord[4][3] = { {0.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

    float minCoord[2] = { 100.0f * width, 100.0f * height }, maxCoord[2] = { -100.0f * width, -100.0f * height };

    int i = 0;

    for (i = 0; i < 4; i++) {
        HRotatePoints(H, imgCoord[i], oImgCoord[i], center[0], center[1]);
    }

    for (i = 0; i < 4; i++) {
        if (oImgCoord[i][0] < minCoord[0])
            minCoord[0] = oImgCoord[i][0];
        if (oImgCoord[i][0] > maxCoord[0])
            maxCoord[0] = oImgCoord[i][0];

        if (oImgCoord[i][1] < minCoord[1])
            minCoord[1] = oImgCoord[i][1];
        if (oImgCoord[i][1] > maxCoord[1])
            maxCoord[1] = oImgCoord[i][1];
    }

    for (i = 0; i < 2; i++) {
        oMinCoord[i] = (int)minCoord[i];
        oMaxCoord[i] = (int)maxCoord[i];
    }

    wh[0] = (int)(maxCoord[0] - minCoord[0] + 1);
    wh[1] = (int)(maxCoord[1] - minCoord[1] + 1);

    return 0;
}

/*
 *           (1) ( (T))
 * H = R + T (-) (N   )
 *           (d) (    )
 * T, N为微量（3*1）, d为焦距，(T)为转置
 * 如果 N = （0， 0， 1）T
 *         0 0 t1/d
 * H = R + 0 0 t2/d
 *         0 0 t3/d
 *参考：http://www.w2bc.com/article/226459
 */
int rt2H(float rx, float ry, float rz,
    float tx, float ty, float tz,
    float f, float s,
    float* H) {
    float Eular[3] = { rx, ry, rz };

#undef FUNC_CODE
#define FUNC_CODE 0x0F

    Euler2RotationMatrix(Eular, H, ORDER_ZXY);

    H[2] += tx * f;
    H[5] += ty * f;
    H[8] += tz;

    H[6] /= f;
    H[7] /= f;

    return 0;
}

int rotateImageH(unsigned char* image, int width, int height, 
                 unsigned char* oImage, int* oWidth, int* oHeight, int* oStride, 
                 float rx, float ry, float rz, 
                 float tx, float ty, float tz,
                 float f, float s) {

#undef FUNC_CODE
#define FUNC_CODE 0x10

    float H[9] = { 0.0f }, H_1[9] = { 0.0f };

    int minCoord[2], maxCoord[2];
    int y;

    float iCoord[2], oCoord[2];
    int wh[2];
    float center[2];

    rt2H(rx, ry, rz, tx, ty, tz, f, s, H);

    Hinverse(H, H_1);

    center[0] = width / 2.0f;
    center[1] = height / 2.0f;
    computeHOutRect(width, height, H, center, minCoord, maxCoord, wh);

    *oWidth = wh[0];
    *oHeight = wh[1];

    if ((*oStride) == 0) {
        *oStride = *oWidth;
    }

    iCoord[0] = width / 2.0f;
    iCoord[1] = height / 2.0f;
    HRotatePoints(H, iCoord, center, width / 2.0f, height / 2.0f);

//#pragma omp parallel for private(y, iCoord) shared(minCoord, H_1, center, width, height, image, oStride) 
    for (y = 0; y < *oHeight; y++) {
        int x;
        for (x = 0; x < *oWidth; x++) {
            iCoord[0] = (float)x + minCoord[0];
            iCoord[1] = (float)y + minCoord[1];

            HRotatePoints(H_1, iCoord, oCoord, center[0], center[1]);

            if (oCoord[0] > 0 && oCoord[0] < width - 1 && oCoord[1] > 0 && oCoord[1] < height - 1) {
                oImage[y * *oStride + x] = getImageValue(image, oCoord[0], oCoord[1], width, height);
            }
            else {
                oImage[y * *oStride + x] = 0;
            }
        }
    }

    return 0;
}

int H_OverlayImageForward(float* H,
    unsigned char* image, int width, int height,
    unsigned char* oImage, int oWidth, int oHeight,
    CenterType type) {

#undef FUNC_CODE
#define FUNC_CODE 0x11

    int y = 0;
    float center[2] = {0.0f};
    if (type == CT_ImageCenter) {
        center[0] = width / 2.0f;
        center[1] = height / 2.0f;
    }

    for (y = 0; y < height; y++) {
        int x;
        float iCoord[2], oCoord[2];
        for (x = 0; x < width; x++) {
            iCoord[0] = (float)x;
            iCoord[1] = (float)y;
            HRotatePoints(H, iCoord, oCoord, center[0], center[1]);

            if (oCoord[0] > 0 && oCoord[0] < oWidth - 1 && oCoord[1] > 0 && oCoord[1] < oHeight - 1) {
                oImage[(int)(oCoord[1]) * oWidth + (int)oCoord[0]] = image[y * width + x];
            }
        }
    }

    return 0;
}

int H_OverlayImageBackward(float* H,
    unsigned char* image, int width, int height,
    unsigned char* oImage, int oWidth, int oHeight,
    CenterType type) {

#undef FUNC_CODE
#define FUNC_CODE 0x12
    float H_1[9];

    int wh[2];
    float center[2] = { 0.0f };
    int minCoord[2], maxCoord[2];

    Hinverse(H, H_1);

    if (type == CT_ImageCenter) {
        center[0] = width / 2.0f;
        center[1] = height / 2.0f;
    }

    computeHOutRect(width, height, H, center, minCoord, maxCoord, wh);

    int y = 0;
    for (y = minCoord[1]; y < maxCoord[1]; y++) {
        if (y >= 0 && y < oHeight) {
            int x;
            float iCoord[2], oCoord[2];
            for (x = minCoord[0]; x < maxCoord[0]; x++) {
                if (x >= 0 && x < oWidth) {
                    iCoord[0] = (float)x;
                    iCoord[1] = (float)y;
                    HRotatePoints(H_1, iCoord, oCoord, center[0], center[1]);

                    if (oCoord[0] >= 0 && oCoord[0] < width && oCoord[1] >= 0 && oCoord[1] < height) {
                        oImage[y * oWidth + x] = getImageValue(image, oCoord[0], oCoord[1], width, height);
                    }
                    else {
                        oImage[y * oWidth + x] = 0;
                    }
                }
            }
        }
    }

    return 0;
}