#ifndef _ALVA_FORMAT_H_
#define _ALVA_FORMAT_H_

/*
 * alvafmt
 * 图像格式
 */
typedef enum _Alva_Format_{
    Alva_FMT_Start              = -1                ,
    Alva_FMT_RGBA_8888                              ,//RGBARGBARGBA
    Alva_FMT_BGRA_8888                              ,//BGRABGRABGRA
    Alva_FMT_GRAY                                   ,//GGGG(gray)
    Alva_FMT_RG_88                                  ,//RGRGRG
    Alva_FMT_RGB_888                                ,//RGBRGBRGB
    Alva_FMT_BGR_888                                ,//BGRBGRBGR
    Alva_FMT_YUV_420p                               ,//Y(w*h) + U(w*h/4) + V(w*h/4)
    Alva_FMT_I420               = Alva_FMT_YUV_420p ,
    Alva_FMT_YU12               = Alva_FMT_YUV_420p ,
    Alva_FMT_YV12                                   ,//Y(w*h) + V(w*h/4) + U(w*h/4)
    Alva_FMT_YUV_422p                               ,//Y(w*h) + U(w*h/2) + V(w*h/2)
    Alva_FMT_YUV_420sp                              ,//Y(w*h) + UV(w*h/4)
    Alva_FMT_NV12               = Alva_FMT_YUV_420sp,
    Alva_FMT_YUV_422sp                              ,//Y(w*h) + UV(w*h/2)
    Alva_FMT_YVU_420sp                              ,//Y(w*h) + VU(w*h/4)
    Alva_FMT_NV21               = Alva_FMT_YVU_420sp,
    Alva_FMT_YVU_422sp                              ,//Y(w*h) + VU(w*h/2)
    
    Alva_FMT_YUVY                                   ,//YUV422, YUYVYUYV
    Alva_FMT_UYVY                                   ,//YUV422, YVYUYVYU

    Alva_FMT_MultiDim                               ,//P1P2P3...PNP1P2P3...PNP1P2P3...PN

    Alva_FMT_NUMBER                                 ,
}alvafmt;

//YUV to RGB formulas
#define CVT_R(y, u, v) (1.164f * ((y) - 16.0f) + 1.596f * ((u) - 128.0f))
#define CVT_G(y, u, v) (1.164f * ((y) - 16.0f) - 0.813f * ((u) - 128.0f) - 0.392f * ((v) - 128.0f))
#define CVT_B(y, u, v) (1.164f * ((y) - 16.0f) + 2.017f * ((u) - 128.0f))

//RGB to YUV formulas
#define CVT_Y(r, g, b) ( 0.257f * (r) + 0.504f * (g) + 0.098f * (b) +  16.0f)
#define CVT_U(r, g, b) (-0.148f * (r) - 0.291f * (g) + 0.439f * (b) + 128.0f)
#define CVT_V(r, g, b) ( 0.439f * (r) - 0.368f * (g) - 0.071f * (b) + 128.0f)

/*
 * alvadt
 * 输入数据类型
 */
typedef enum _Alva_DataType_{
    Alva_Memory                 = 0                 , //后续输入为内存
    Alva_Texture                = 1                 , //后续输入为OpenGL纹理
}alvadt;

/*
 * alvacf
 * 输入数据的Camera朝向
 */
typedef enum _Alva_CameraFacing_ {
    Alva_BACK                   = 0                 , //后置摄像头
    Alva_FRONT                  = 1                 , //前置摄像头
}alvacf;

/*
 * alvacf
 * 输入路径的指向类型
 */
typedef enum _Alva_FileType_ {
    Alva_Asset                  = 0                 , //Android的Asset文件（仅限Android平台）
    Alva_File                   = 1                 , //存储器中的文件
}alvaft;

#endif