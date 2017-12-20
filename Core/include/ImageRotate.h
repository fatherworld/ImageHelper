#ifndef _Image_Rotate_H_
#define _Image_Rotate_H_

#ifdef AlvaPublic
#include "ImageHelper_Hide.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    int matrix_matrix(float* a, float* b, float* result, int dim);

    int vector_matrix(float* v, float* m, float* vresult, int dim);

    int matrix_vector(float* m, float* v, float* vresult, int dim);

    void ComputeRotateMatrix(float sita, float* m_sita);

    void ComputeInverseRotateMatrix(float sita, float* m_sita);

    int computeAffineMatrix(float sita, float fai, float lambd1, float lambd2, float* A);

    int computeInverseAffineMatrix(float sita, float fai, float lambd1, float lambd2, float* IA);

    /*
     * computeOutRect
     * 根据输入宽高及旋转参数计算输出宽高
     */
    int computeOutRect(int width, int height, float sita, float fai, float lambd1, float lambd2, int* oMinCoord, int* oMaxCoord, int* wh);

    /*
     * rotateImage
     * 旋转图像，采用经纬度旋转
     * 即：An afﬁne transformation can be decomposed as A = RθRφ(−1)SRφ, 
     *     where Rθ and Rφ are two rotation matrices respectively parameterized by the angles θ and φ, 
     *     and S = diag[λ1,λ2] is a scaling matrix
     * @param image: 输入图像（灰度图）
     * @param width: 输入图像宽
     * @param height: 输入图像高
     * @param oImage: 输出图像（灰度图)
     * @param oWidth: 输出图像宽
     * @param oHeight: 输出图像高
     * @param oStride: 输出图像对齐，如果输入不为0，则按输入的对齐输出图像，如果输入为0，则等于输出宽
     * @param sita: 旋转量（单位角度）
     * @param fai: 旋转量（单位角度）
     * @param lambd1: 缩放量
     * @param lambd2: 缩放量
     */
    int rotateImage(unsigned char* image, int width, int height, unsigned char* oImage, int* oWidth, int* oHeight, int* oStride, float sita, float fai, float lambd1, float lambd2);

    //单应性旋转
    /*
     * rt2H
     * 生成单应性矩阵
     * @param rx: x轴旋转量
     * @param ry: y轴旋转量
     * @param rz: z轴旋转量
     * @param tx: x轴平移量
     * @param ty: y轴平移量
     * @param tz: z轴平移量，在参考f的情况下，-0.5图像放大2倍，0.5图像缩小到原来一半
     * @param f : 焦距，参考：1303.0f
     * @param s : 缩放量，暂时无用
     * @param H : H
     */
    int rt2H(float rx, float ry, float rz,
             float tx, float ty, float tz,
             float f, float s,
             float* H);

    int Hinverse(float* H, float* Hinverse);

    int HRotatePoints(float* H, float* inPoint, float* outPoint, float cx, float cy);

    int computeHOutRect(int width, int height, float* H, float* center, int* oMinCoord, int* oMaxCoord, int* wh);

    int rotateImageH(unsigned char* image, int width, int height,
                     unsigned char* oImage, int* oWidth, int* oHeight, int* oStride,
                     float rx, float ry, float rz,
                     float tx, float ty, float tz,
                     float f , float s);


    typedef enum _CenterType_ {
        CT_LeftUpCorner        = 0,
        CT_ImageCenter       = 1,
    }CenterType;

    int H_OverlayImageForward(float* H,
        unsigned char* image, int width, int height,
        unsigned char* oImage, int oWidth, int oHeight,
        CenterType type);

    int H_OverlayImageBackward(float* H,
        unsigned char* image, int width, int height,
        unsigned char* oImage, int oWidth, int oHeight,
        CenterType type);
#ifdef __cplusplus
}
#endif
#endif