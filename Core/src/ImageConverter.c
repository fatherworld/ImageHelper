#include "Config.h"

#include <stdlib.h>
#include "stdio_fl.h"
#include "memory_fl.h"

#include "ImageConverter.h"

#undef FILE_NUM
#define FILE_NUM 0xF300

#undef AVS_Clamp
#define AVS_Clamp(x, minValue, maxValue) ((x < minValue) ? (minValue) : ((x > maxValue) ? (maxValue) : (x)))

#if 1  
/****************************RGBA  2   ...*******************/
static int RGBA2BGRA(unsigned char* RGBA, unsigned char* BGRA, int width, int height)
{

#undef FUNC_CODE
#define FUNC_CODE 0x01

    int i = 0;
    for (i = 0; i < width * height * 4; i += 4)
    {
        BGRA[i] = RGBA[i + 2];
        BGRA[i + 1] = RGBA[i + 1];
        BGRA[i + 2] = RGBA[i];
        BGRA[i + 3] = RGBA[i + 3];
    }
    return 0;
}

static int RGBA2RGB(unsigned char* RGBA, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
    {
        RGB[j] = RGBA[i];
        RGB[j + 1] = RGBA[i + 1];
        RGB[j + 2] = RGBA[i + 2];
    }
    return 0;
}

static int RGBA2BGR(unsigned char* RGBA, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x03

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
    {
        BGR[j] = RGBA[i + 2];
        BGR[j + 1] = RGBA[i + 1];
        BGR[j + 2] = RGBA[i];
    }
    return 0;
}

/****YUV420P_I420_YU12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	V1	V2	V3	V4	V5	V6	V7	V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2_YUV420P_I420_YU12(unsigned char* RGBA, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x04

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2YV12(unsigned char* RGBA, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x05

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422p存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	U9	U10	U11	U12	U13	U14	U15	U16
*	V1	V2	V3	V4	V5	V6	V7	V8
*	V9	V10	V11	V12	V13	V14	V15	V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2YUV422P(unsigned char* RGBA, unsigned char* YUV422P, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x06

    int i = 0, j = 0;
    unsigned char* _Y = YUV422P;
    unsigned char* _U = YUV422P + width * height;
    unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2 U3V3 U4V4	U5V5 U6V6 U7V7 U8V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2_YUV420sp_NV12(unsigned char* RGBA, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x07

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2   U3V3   U4V4	  U5V5   U6V6   U7V7   U8V8
*	U9V9 U10V10 U11V11 U12V12 U13V13 U14V14 U15V15 U16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2YUV422sp(unsigned char* RGBA, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x08

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2 V3U3 V4U4	V5U5 V6U6 V7U7 V8U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2_YVU420sp_NV21(unsigned char* RGBA, unsigned char* YVU420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x09

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV12;
    unsigned char* _VU = YVU420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YVU422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2   V3U3   V4U4	  V5U5   V6U6   V7U7   V8U8
*	V9U9 V10U10 V11U11 V12U12 V13U13 V14U14 V15U15 V16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2YVU422sp(unsigned char* RGBA, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0A

    int i = 0, j = 0;
    unsigned char* _Y = YVU422sp;
    unsigned char* _VU = YVU422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2YUVY(unsigned char* RGBA, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0B

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****UYVY存储格式****
*	Y1V1	Y2U2	Y3V3	Y4U4	Y5V5	Y6U6	Y7V7	Y8U8
*	Y9V9	Y10U10	Y11V11	Y12U12	Y13V13	Y14U14  Y15V15	Y16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGBA2UYVY(unsigned char* RGBA, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0C

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGBA[(j * width + i) * 4];
            G = RGBA[(j * width + i) * 4 + 1];
            B = RGBA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 1)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

static int RGBA2Gray(unsigned char* RGBA, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0D

    int i = 0;
#pragma omp parallel for private(i) shared(width, height, Gray) 
    for (i = 0; i < width * height * 4; i += 4)
    {
        int R = 0, G = 0, B = 0;
        float Y = 0.0f;
        R = RGBA[i];
        G = RGBA[i + 1];
        B = RGBA[i + 2];
        Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
        Gray[i / 4] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
    }
    return 0;
}

#endif

#if 1
/******************BGRA   2    ... ****************/
static int BGRA2RGBA(unsigned char* BGRA, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0E

    int i = 0;
    for (i = 0; i < width * height * 4; i += 4)
    {
        RGBA[i] = BGRA[i + 2];
        RGBA[i + 1] = BGRA[i + 1];
        RGBA[i + 2] = BGRA[i];
        RGBA[i + 3] = BGRA[i + 3];
    }
    return 0;
}

static int BGRA2RGB(unsigned char* BGRA, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x0F

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
    {
        RGB[j] = BGRA[i + 2];
        RGB[j + 1] = BGRA[i + 1];
        RGB[j + 2] = BGRA[i];
    }
    return 0;
}

static int BGRA2BGR(unsigned char* BGRA, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x10

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
    {
        BGR[j] = BGRA[i];
        BGR[j + 1] = BGRA[i + 1];
        BGR[j + 2] = BGRA[i + 2];
    }
    return 0;
}

/****YUV420P_I420_YU12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	V1	V2	V3	V4	V5	V6	V7	V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2_YUV420P_I420_YU12(unsigned char* BGRA, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x11

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2YV12(unsigned char* BGRA, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x12

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422p存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	U9	U10	U11	U12	U13	U14	U15	U16
*	V1	V2	V3	V4	V5	V6	V7	V8
*	V9	V10	V11	V12	V13	V14	V15	V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2YUV422P(unsigned char* BGRA, unsigned char* YUV422P, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x13

    int i = 0, j = 0;
    unsigned char* _Y = YUV422P;
    unsigned char* _U = YUV422P + width * height;
    unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2 U3V3 U4V4	U5V5 U6V6 U7V7 U8V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2_YUV420sp_NV12(unsigned char* BGRA, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x14

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2   U3V3   U4V4	  U5V5   U6V6   U7V7   U8V8
*	U9V9 U10V10 U11V11 U12V12 U13V13 U14V14 U15V15 U16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2YUV422sp(unsigned char* BGRA, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x15

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2 V3U3 V4U4	V5U5 V6U6 V7U7 V8U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2_YVU420sp_NV21(unsigned char* BGRA, unsigned char* YVU420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x16

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV12;
    unsigned char* _VU = YVU420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YVU422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2   V3U3   V4U4	  V5U5   V6U6   V7U7   V8U8
*	V9U9 V10U10 V11U11 V12U12 V13U13 V14U14 V15U15 V16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2YVU422sp(unsigned char* BGRA, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x17

    int i = 0, j = 0;
    unsigned char* _Y = YVU422sp;
    unsigned char* _VU = YVU422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2YUVY(unsigned char* BGRA, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x18

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****UYVY存储格式****
*	Y1V1	Y2U2	Y3V3	Y4U4	Y5V5	Y6U6	Y7V7	Y8U8
*	Y9V9	Y10U10	Y11V11	Y12U12	Y13V13	Y14U14  Y15V15	Y16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGRA2UYVY(unsigned char* BGRA, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x19

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGRA[(j * width + i) * 4];
            G = BGRA[(j * width + i) * 4 + 1];
            R = BGRA[(j * width + i) * 4 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 1)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

static int BGRA2Gray(unsigned char* BGRA, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1A

    int i = 0;
    for (i = 0; i < width * height * 4; i += 4)
    {
        int R = 0, G = 0, B = 0;
        float Y = 0.0f;
        B = BGRA[i];
        G = BGRA[i + 1];
        R = BGRA[i + 2];
        Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
        Gray[i / 4] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
    }
    return 0;
}
#endif

#if 1
/***************** RGB 2 ... ********/
static int RGB2RGBA(unsigned char* RGB, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1B

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
    {
        RGBA[j] = RGB[i];
        RGBA[j + 1] = RGB[i + 1];
        RGBA[j + 2] = RGB[i + 2];
        RGBA[j + 3] = 255;
    }
    return 0;
}

static int RGB2BGRA(unsigned char* RGB, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1C

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
    {
        BGRA[j] = RGB[i + 2];
        BGRA[j + 1] = RGB[i + 1];
        BGRA[j + 2] = RGB[i];
        BGRA[j + 3] = 255;
    }
    return 0;
}

static int RGB2BGR(unsigned char* RGB, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1D

    int i = 0;
    for (i = 0; i < width * height * 3; i += 3)
    {
        BGR[i] = RGB[i + 2];
        BGR[i + 1] = RGB[i + 1];
        BGR[i + 2] = RGB[i];
    }
    return 0;
}

/****YUV420P_I420_YU12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	V1	V2	V3	V4	V5	V6	V7	V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2_YUV420P_I420_YU12(unsigned char* RGB, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1E

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2YV12(unsigned char* RGB, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x1F

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422p存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	U9	U10	U11	U12	U13	U14	U15	U16
*	V1	V2	V3	V4	V5	V6	V7	V8
*	V9	V10	V11	V12	V13	V14	V15	V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2YUV422P(unsigned char* RGB, unsigned char* YUV422P, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x20

    int i = 0, j = 0;
    unsigned char* _Y = YUV422P;
    unsigned char* _U = YUV422P + width * height;
    unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2 U3V3 U4V4	U5V5 U6V6 U7V7 U8V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2_YUV420sp_NV12(unsigned char* RGB, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x21

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2   U3V3   U4V4	  U5V5   U6V6   U7V7   U8V8
*	U9V9 U10V10 U11V11 U12V12 U13V13 U14V14 U15V15 U16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2YUV422sp(unsigned char* RGB, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x22

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2 V3U3 V4U4	V5U5 V6U6 V7U7 V8U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2_YVU420sp_NV21(unsigned char* RGB, unsigned char* YVU420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x23

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV12;
    unsigned char* _VU = YVU420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YVU422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2   V3U3   V4U4	  V5U5   V6U6   V7U7   V8U8
*	V9U9 V10U10 V11U11 V12U12 V13U13 V14U14 V15U15 V16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2YVU422sp(unsigned char* RGB, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x24

    int i = 0, j = 0;
    unsigned char* _Y = YVU422sp;
    unsigned char* _VU = YVU422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2YUVY(unsigned char* RGB, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x25

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****UYVY存储格式****
*	Y1V1	Y2U2	Y3V3	Y4U4	Y5V5	Y6U6	Y7V7	Y8U8
*	Y9V9	Y10U10	Y11V11	Y12U12	Y13V13	Y14U14  Y15V15	Y16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int RGB2UYVY(unsigned char* RGB, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x26

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            R = RGB[(j * width + i) * 3];
            G = RGB[(j * width + i) * 3 + 1];
            B = RGB[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 1)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

static int RGB2Gray(unsigned char* RGB, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x27

    int i = 0;
    for (i = 0; i < width * height * 3; i += 3)
    {
        int R = 0, G = 0, B = 0;
        float Y = 0.0f;
        R = RGB[i];
        G = RGB[i + 1];
        B = RGB[i + 2];
        Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
        Gray[i / 3] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
    }
    return 0;
}
#endif

#if 1
/****************  BGR   2   ...  */
static int BGR2RGBA(unsigned char* BGR, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x28

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
    {
        RGBA[j] = BGR[i + 2];
        RGBA[j + 1] = BGR[i + 1];
        RGBA[j + 2] = BGR[i];
        RGBA[j + 3] = 255;
    }
    return 0;
}

static int BGR2BGRA(unsigned char* BGR, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x29

    int i = 0, j = 0;
    for (i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
    {
        BGRA[j] = BGR[i];
        BGRA[j + 1] = BGR[i + 1];
        BGRA[j + 2] = BGR[i + 2];
        BGRA[j + 3] = 255;
    }
    return 0;
}

static int BGR2RGB(unsigned char* BGR, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2A

    int i = 0;
    for (i = 0; i < width * height * 3; i += 3)
    {
        RGB[i] = BGR[i + 2];
        RGB[i + 1] = BGR[i + 1];
        RGB[i + 2] = BGR[i];
    }
    return 0;
}

/****YUV420P_I420_YU12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	V1	V2	V3	V4	V5	V6	V7	V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2_YUV420P_I420_YU12(unsigned char* BGR, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2B

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2YV12(unsigned char* BGR, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2C

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[(j / 2) * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422p存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	U9	U10	U11	U12	U13	U14	U15	U16
*	V1	V2	V3	V4	V5	V6	V7	V8
*	V9	V10	V11	V12	V13	V14	V15	V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2YUV422P(unsigned char* BGR, unsigned char* YUV422P, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2D

    int i = 0, j = 0;
    unsigned char* _Y = YUV422P;
    unsigned char* _U = YUV422P + width * height;
    unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _U[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _V[j * ((width + 1) / 2) + i / 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2 U3V3 U4V4	U5V5 U6V6 U7V7 U8V8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2_YUV420sp_NV12(unsigned char* BGR, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2E

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YUV422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2   U3V3   U4V4	  U5V5   U6V6   U7V7   U8V8
*	U9V9 U10V10 U11V11 U12V12 U13V13 U14V14 U15V15 U16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2YUV422sp(unsigned char* BGR, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x2F

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _UV[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)U, 0, 255);
                _UV[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

/****YVU420sp NV21存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2 V3U3 V4U4	V5U5 V6U6 V7U7 V8U8
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2_YVU420sp_NV21(unsigned char* BGR, unsigned char* YVU420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x30

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV12;
    unsigned char* _VU = YVU420sp_NV12 + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YVU422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2   V3U3   V4U4	  V5U5   V6U6   V7U7   V8U8
*	V9U9 V10U10 V11U11 V12U12 V13U13 V14U14 V15U15 V16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2YVU422sp(unsigned char* BGR, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x31

    int i = 0, j = 0;
    unsigned char* _Y = YVU422sp;
    unsigned char* _VU = YVU422sp + width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            _Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 0)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                _VU[(j * ((width + 1) / 2) + i / 2) * 2] = (unsigned char)AVS_Clamp((int)V, 0, 255);
                _VU[(j * ((width + 1) / 2) + i / 2) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
        }
    }
    return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/


static int BGR2YUVY(unsigned char* BGR, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x32
     int i = 0, j = 0;
     for (j = 0; j < height; j++)
     {
         for (i = 0; i < width; i++)
         {
             int R = 0, G = 0, B = 0;
             float Y = 0.0f, U = 0.0f, V = 0.0f;
             B = BGR[(j * width + i)];
             G = BGR[(j * width + i) + 1];
             R = BGR[(j * width + i) + 2];
             Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
             YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
             if (i % 2 == 0)
             {
                 U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                 YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
             }
             else
             {
                 V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                 YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
             }
         }
     }
    return 0;
}


/****UYVY存储格式****
*	Y1V1	Y2U2	Y3V3	Y4U4	Y5V5	Y6U6	Y7V7	Y8U8
*	Y9V9	Y10U10	Y11V11	Y12U12	Y13V13	Y14U14  Y15V15	Y16U16
******************************************
****转换公式**********************
*   Y =  0.299 * R + 0.587 * G + 0.114 * B
*   U = -0.147 * R - 0.289 * G + 0.436 * B = 0.492 * (B - Y)
*   V =  0.615 * R - 0.515 * G - 0.100 * B = 0.877 * (R - Y)
*/
static int BGR2UYVY(unsigned char* BGR, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x33

    int i = 0, j = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            int R = 0, G = 0, B = 0;
            float Y = 0.0f, U = 0.0f, V = 0.0f;
            B = BGR[(j * width + i) * 3];
            G = BGR[(j * width + i) * 3 + 1];
            R = BGR[(j * width + i) * 3 + 2];
            Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
            YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
            if (i % 2 == 1)
            {
                U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
            }
            else
            {
                V = 0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
                YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
            }
        }
    }
    return 0;
}

static int BGR2Gray(unsigned char* BGR, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x34

    int i = 0;
    for (i = 0; i < width * height * 3; i += 3)
    {
        int R = 0, G = 0, B = 0;
        float Y = 0.0f;
        B = BGR[i];
        G = BGR[i + 1];
        R = BGR[i + 2];
        Y = 0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
        Gray[(i / 3) + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
    }
    return 0;
}

#endif

#if 1
/*************** YUV420P_I420_YU12  2  ... *********/
/****YUV420P_I420_YU12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	V1	V2	V3	V4	V5	V6	V7	V8
***************************************
********转换公式***********************
* R = Y + 1.140 * V
* G = Y - 0.394 * U - 0.581 * V
* B = Y + 2.032 * U
*/
static int YUV420P_I420_YU12_2_RGBA(unsigned char* YUV420P_I420_YU12, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x35

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}




static int YUV420P_I420_YU12_2_BGRA(unsigned char* YUV420P_I420_YU12, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x36

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV420P_I420_YU12_2_RGB(unsigned char* YUV420P_I420_YU12, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x37

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}



static int YUV420P_I420_YU12_2_BGR(unsigned char* YUV420P_I420_YU12, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x38
    int i = 0, j = 0;
    unsigned char* _Y_cur= YUV420P_I420_YU12;
    unsigned char* _Y_nxt = _Y_cur + width;
    unsigned char* _U_cur = YUV420P_I420_YU12 + width * height;
    unsigned char* _V_cur = YUV420P_I420_YU12 + width * height * 5 / 4;
    for (j = 0; j < height/2; j++)
    {
        for (i = 0; i < width/2; i++)
        {
              (*BGR++)= (float)((int)(*_Y_cur)) + 2.032f * ((float)((int)(*_U_cur)));
              (*BGR++)= (float)((int)(*_Y_cur)) - 0.394f * (float)((int)(*_U_cur)) - 0.581f * (float)((int)(*_V_cur));
              (*BGR++) = (float)((int)(*_Y_cur)) + 1.140f * (float)((int)(*_U_cur));
               _Y_cur++;
               (*BGR++) = (float)((int)(*_Y_cur)) + 2.032f * ((float)((int)(*_U_cur)));
               (*BGR++) = (float)((int)(*_Y_cur)) - 0.394f * (float)((int)(*_U_cur)) - 0.581f * (float)((int)(*_V_cur));
               (*BGR++) = (float)((int)(*_Y_cur)) + 1.140f * (float)((int)(*_U_cur));
             
               (*BGR++) = (float)((int)(*_Y_nxt)) + 2.032f * ((float)((int)(*_U_cur)));
               (*BGR++) = (float)((int)(*_Y_nxt)) - 0.394f * (float)((int)(*_U_cur)) - 0.581f * (float)((int)(*_V_cur));
               (*BGR++) = (float)((int)(*_Y_nxt)) + 1.140f * (float)((int)(*_U_cur));

               _Y_nxt++;
               (*BGR++) = (float)((int)(*_Y_nxt)) + 2.032f * ((float)((int)(*_U_cur)));
               (*BGR++) = (float)((int)(*_Y_nxt)) - 0.394f * (float)((int)(*_U_cur)) - 0.581f * (float)((int)(*_V_cur));
               (*BGR++) = (float)((int)(*_Y_nxt)) + 1.140f * (float)((int)(*_U_cur));
        }
        _Y_cur += width << 1;
        _Y_nxt += width << 1;
        _U_cur++;
        _V_cur++;
    }
    return 0;
}

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************/
static int YUV420P_I420_YU12_2_YV12(unsigned char* YUV420P_I420_YU12, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x39

    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YV12;
    unsigned char* __V = YV12 + width * height;
    unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    memcpy_fl(__V, sizeof(unsigned char) * width * height, _U, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2));
    memcpy_fl(__U, sizeof(unsigned char) * width * height, _V, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2));
    return 0;
}

/****YUV422p存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1	U2	U3	U4	U5	U6	U7	U8
*	U9	U10	U11	U12	U13	U14	U15	U16
*	V1	V2	V3	V4	V5	V6	V7	V8
*	V9	V10	V11	V12	V13	V14	V15	V16
******************************************/
static int YUV420P_I420_YU12_2_YUV422p(unsigned char* YUV420P_I420_YU12, unsigned char* YUV422p, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3A

    int i = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV422p;
    unsigned char* __U = YUV422p + width * height;
    unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < height; i++)
    {
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
    }
    return 0;
}

/****YUV420sp NV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1 U2V2 U3V3 U4V4	U5V5 U6V6 U7V7 U8V8
******************************************/
static int YUV420P_I420_YU12_2_YUV420sp_NV12(unsigned char* YUV420P_I420_YU12, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3B

    int i = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV420sp_NV12;
    unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < ((width + 1) / 2) * ((height + 1) / 2); i++)
    {
        __UV[i * 2] = _U[i];
        __UV[i * 2 + 1] = _V[i];
    }
    return 0;
}

/****YUV422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	U1V1   U2V2   U3V3   U4V4
*   U5V5   U6V6   U7V7   U8V8
*	U9V9   U10V10 U11V11 U12V12
*   U13V13 U14V14 U15V15 U16V16
******************************************/
static int YUV420P_I420_YU12_2_YUV422sp(unsigned char* YUV420P_I420_YU12, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3C

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV422sp;
    unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __UV[(j * ((width + 1) / 2) + i) * 2] = _U[(j / 2) * ((width + 1) / 2) + i];
            __UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

/****YVU420sp NV21存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2 V3U3 V4U4	V5U5 V6U6 V7U7 V8U8
******************************************/
static int YUV420P_I420_YU12_2_YVU420sp_NV21(unsigned char* YUV420P_I420_YU12, unsigned char* YVU420sp_NV21, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3D

    int i = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YVU420sp_NV21;
    unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < ((width + 1) / 2) * (height + 1) / 2; i++)
    {
        __VU[i * 2] = _V[i];
        __VU[i * 2 + 1] = _U[i];
    }
    return 0;
}

/****YVU422sp存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1U1 V2U2   V3U3   V4U4	  V5U5   V6U6   V7U7   V8U8
*	V9U9 V10U10 V11U11 V12U12 V13U13 V14U14 V15U15 V16U16
******************************************/
static int YUV420P_I420_YU12_2_YVU422sp(unsigned char* YUV420P_I420_YU12, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3E

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _V[(j / 2) * ((width + 1) / 2) + i];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************/
static int YUV420P_I420_YU12_2_YUVY(unsigned char* YUV420P_I420_YU12, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x3F

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
        }
    }
    return 0;
}

/****UYVY存储格式****
*	Y1V1	Y2U2	Y3V3	Y4U4	Y5V5	Y6U6	Y7V7	Y8U8
*	Y9V9	Y10U10	Y11V11	Y12U12	Y13V13	Y14U14  Y15V15	Y16U16
******************************************/
static int YUV420P_I420_YU12_2_UYVY(unsigned char* YUV420P_I420_YU12, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x40

    int i = 0, j = 0;
    unsigned char* _Y = YUV420P_I420_YU12;
    unsigned char* _U = YUV420P_I420_YU12 + width * height;
    unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
        }
    }
    return 0;
}

static int YUV420P_I420_YU12_2_Gray(unsigned char* YUV420P_I420_YU12, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x41

    memcpy_fl(Gray, sizeof(unsigned char) * width * height, YUV420P_I420_YU12, sizeof(unsigned char) * width * height);
    return 0;
}
#endif

#if 1
/************** YV12  2  ...  *******/
/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
*****************************************
********转换公式***********************
* R = Y + 1.140 * V
* G = Y - 0.394 * U - 0.581 * V
* B = Y + 2.032 * U
*/
static int YV12_2RGBA(unsigned char* YV12, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x42

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }

    return 0;
}

static int YV12_2BGRA(unsigned char* YV12, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x43

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YV12_2RGB(unsigned char* YV12, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x44

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}

static int YV12_2BGR(unsigned char* YV12, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x45

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            V = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGR[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGR[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGR[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
        }
    }
    return 0;
}

static int YV12_2_YUV420p_I420_YU12(unsigned char* YV12, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x46

    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV420P_I420_YU12;
    unsigned char* __U = YUV420P_I420_YU12 + width * height;
    unsigned char* __V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    memcpy_fl(__U, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2), _U, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2));
    memcpy_fl(__V, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2), _V, sizeof(unsigned char) * ((width + 1) / 2) * ((height + 1) / 2));

    return 0;
}

static int YV12_2YUV422p(unsigned char* YV12, unsigned char* YUV422p, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x47

    int i = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV422p;
    unsigned char* __U = YUV422p + width * height;
    unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < height; i++)
    {
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
    }
    return 0;
}

static int YV12_2_YUV420sp_NV12(unsigned char* YV12, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x48

    int i = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV420sp_NV12;
    unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < ((width + 1) / 2) * ((height + 1) / 2); i++)
    {
        __UV[i * 2] = _U[i];
        __UV[i * 2 + 1] = _V[i];
    }
    return 0;
}

static int YV12_2YUV422sp(unsigned char* YV12, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x49

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YUV422sp;
    unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __UV[(j * ((width + 1) / 2) + i) * 2] = _U[(j / 2) * ((width + 1) / 2) + i];
            __UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + i];
        }
    }

    return 0;
}

static int YV12_2_YVU420sp_NV21(unsigned char* YV12, unsigned char* YVU420sp_NV21, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4A

    int i = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YVU420sp_NV21;
    unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (i = 0; i < ((width + 1) / 2) * (height + 1) / 2; i++)
    {
        __VU[i * 2] = _V[i];
        __VU[i * 2 + 1] = _U[i];
    }
    return 0;
}

static int YV12_2YVU422sp(unsigned char* YV12, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4B

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _V[(j / 2) * ((width + 1) / 2) + i];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

static int YV12_2YUVY(unsigned char* YV12, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4C

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
        }
    }

    return 0;
}

static int YV12_2UYVY(unsigned char* YV12, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4D

    int i = 0, j = 0;
    unsigned char* _Y = YV12;
    unsigned char* _V = YV12 + width * height;
    unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
            }
        }
    }
    return 0;
}

static int YV12_2Gray(unsigned char* YV12, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4E

    memcpy_fl(Gray, sizeof(unsigned char) * width * height, YV12, sizeof(unsigned char) * width * height);
    return 0;
}

#endif

#if 1
/****************** YUV422p  2  ...  *******/
static int YUV422p_2RGBA(unsigned char* YUV422p, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x4F

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[j * ((width + 1) / 2) + (i / 2)];
            V = _V[j * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV422p_2BGRA(unsigned char* YUV422p, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x50

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[j * ((width + 1) / 2) + (i / 2)];
            V = _V[j * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV422p_2RGB(unsigned char* YUV422p, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x51

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[j * ((width + 1) / 2) + (i / 2)];
            V = _V[j * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}

static int YUV422p_2BGR(unsigned char* YUV422p, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x52

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _U[j * ((width + 1) / 2) + (i / 2)];
            V = _V[j * ((width + 1) / 2) + (i / 2)];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGR[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGR[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGR[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
        }
    }
    return 0;
}

static int YUV422p_2_YUV420p_I420_YU12(unsigned char* YUV422p, unsigned char* YUV420P_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x52

    int i = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YUV420P_I420_YU12;
    unsigned char* __U = YUV420P_I420_YU12 + width * height;
    unsigned char* __V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < (height + 1) / 2; i++)
    {
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
    }
    return 0;
}

static int YUV422p_2YV12(unsigned char* YUV422p, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x53

    int i = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YV12;
    unsigned char* __V = YV12 + width * height;
    unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < (height + 1) / 2; i++)
    {
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
    }
    return 0;
}

static int YUV422p_2_YUV420sp_NV12(unsigned char* YUV422p, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x54

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YUV420sp_NV12;
    unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __UV[(j * ((width + 1) / 2) + i) * 2 + 0] = _U[(j * 2) * ((width + 1) / 2) + i];
            __UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _V[(j * 2) * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

static int YUV422p_2YUV422sp(unsigned char* YUV422p, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x55

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YUV422sp;
    unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __UV[(j * ((width + 1) / 2)) * 2 + 0] = _U[j * ((width + 1) / 2) + i];
            __UV[(j * ((width + 1) / 2)) * 2 + 1] = _V[j * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

static int YUV422p_2_YVU420sp_NV21(unsigned char* YUV422p, unsigned char* YVU420sp_NV21, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x56

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YVU420sp_NV21;
    unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2)) * 2 + 0] = _V[j * 2 * ((width + 1) / 2) + i];
            __VU[(j * ((width + 1) / 2)) * 2 + 1] = _U[j * 2 * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

static int YUV422p_2YVU422sp(unsigned char* YUV422p, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x57

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2)) * 2 + 0] = _V[j * ((width + 1) / 2) + i];
            __VU[(j * ((width + 1) / 2)) * 2 + 1] = _U[j * ((width + 1) / 2) + i];
        }
    }
    return 0;
}

static int YUV422p_2YUVY(unsigned char* YUV422p, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x58

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _U[j * ((width + 1) / 2) + i / 2];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _V[j * ((width + 1) / 2) + i / 2];
            }
        }
    }
    return 0;
}

static int YUV422p_2UYVY(unsigned char* YUV422p, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x59

    int i = 0, j = 0;
    unsigned char* _Y = YUV422p;
    unsigned char* _U = YUV422p + width * height;
    unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _U[j * ((width + 1) / 2) + i / 2];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _V[j * ((width + 1) / 2) + i / 2];
            }
        }
    }
    return 0;
}

static int YUV422p_2Gray(unsigned char* YUV422p, unsigned char* Gray, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5A

    memcpy_fl(Gray, sizeof(unsigned char) * width * height, YUV422p, sizeof(unsigned char) * width * height);
    return 0;
}

#endif

#if 1
/********************* YUV420sp_NV12  2  ... *********/
static int YUV420sp_NV12_2RGBA(unsigned char* YUV420sp_NV12, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5B

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV420sp_NV12_2BGRA(unsigned char* YUV420sp_NV12, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5C

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV420sp_NV12_2RGB(unsigned char* YUV420sp_NV12, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5D

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}

static int YUV420sp_NV12_2BGR(unsigned char* YUV420sp_NV12, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5E

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGR[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGR[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGR[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
        }
    }
    return 0;
}

static int YUV420sp_NV12_2_YUV420p_I420_YU12(unsigned char* YUV420sp_NV12, unsigned char* YUV420p_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x5F

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YUV420p_I420_YU12;
    unsigned char* __U = YUV420p_I420_YU12 + width * height;
    unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2];
            __V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV420sp_NV12_2YV12(unsigned char* YUV420sp_NV12, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x60

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YV12;
    unsigned char* __V = YV12 + width * height;
    unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2];
            __V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV420sp_NV12_2YUV422p(unsigned char* YUV420sp_NV12, unsigned char* YUV422p, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x61

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YUV422p;
    unsigned char* __U = YUV422p + width * height;
    unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 0];
            __V[j * ((width + 1) / 2) + i] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV420sp_NV12_2YUV422sp(unsigned char* YUV420sp_NV12, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x62

    int i = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YUV422sp;
    unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (i = 0; i < height; i++)
    {
        memcpy_fl(&(__UV[i * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2, &(_UV[(i / 2) * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2);
    }
    return 0;
}

static int YUV420sp_NV12_2_YVU420sp_NV21(unsigned char* YUV420sp_NV12, unsigned char* YVU420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x63

    int i = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YVU420sp_NV12;
    unsigned char* __VU = YVU420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (i = 0; i < (((width + 1) / 2) * ((height + 1) / 2)); i++)
    {
        __VU[i * 2] = _UV[i * 2 + 1];
        __VU[i * 2 + 1] = _UV[i * 2];
    }
    return 0;
}

static int YUV420sp_NV12_2YVU422sp(unsigned char* YUV420sp_NV12, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x64

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YUV420sp_NV12_2YUVY(unsigned char* YUV420sp_NV12, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x65

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
        }
    }
    return 0;
}

static int YUV420sp_NV12_2UYVY(unsigned char* YUV420sp_NV12, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x66

    int i = 0, j = 0;
    unsigned char* _Y = YUV420sp_NV12;
    unsigned char* _UV = YUV420sp_NV12 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
        }
    }
    return 0;
}

static int YUV420sp_NV12_2Gray(unsigned char* YUV420sp_NV12, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x67

    memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YUV420sp_NV12, sizeof(unsigned char) * width * height);
    return 0;
}

#endif

#if 1
/************ YUV422sp  2  ... ********/
static int YUV422sp_2RGBA(unsigned char* YUV422sp, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x68

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV422sp_2BGRA(unsigned char* YUV422sp, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x69

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YUV422sp_2RGB(unsigned char* YUV422sp, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6A

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}

static int YUV422sp_2BGR(unsigned char* YUV422sp, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6B

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            V = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGR[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGR[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGR[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
        }
    }
    return 0;
}

static int YUV422sp_2_YUV420p_I420_YU12(unsigned char* YUV422sp, unsigned char* YUV420p_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6C

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YUV420p_I420_YU12;
    unsigned char* __U = YUV420p_I420_YU12 + width * height;
    unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2];
            __V[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV422sp_2YV12(unsigned char* YUV422sp, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6D

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YV12;
    unsigned char* __V = YV12 + width * height;
    unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2];
            __V[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV422sp_2YUV422p(unsigned char* YUV422sp, unsigned char* YUV422p, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6E

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YUV422p;
    unsigned char* __U = YUV422p + width * height;
    unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 0];
            __V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
        }
    }
    return 0;
}

static int YUV422sp_2_YUV420sp_NV12(unsigned char* YUV422sp, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x6F

    int i = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YUV420sp_NV12;
    unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (i = 0; i < (height + 1) / 2; i++)
    {
        memcpy_fl(&(__UV[i * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2, &(_UV[(i * 2) * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2);
    }
    return 0;
}

static int YUV422sp_2_YVU420sp_NV21(unsigned char* YUV422sp, unsigned char* YVU420sp_NV21, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x70

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YVU420sp_NV21;
    unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < ((width + 1) / 2); i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _UV[((j * 2) * ((width + 1) / 2) + i) * 2 + 1];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[((j * 2) * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YUV422sp_2YVU422sp(unsigned char* YUV422sp, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x71

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YUV422sp_2YUVY(unsigned char* YUV422sp, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x72

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
        }
    }
    return 0;
}

static int YUV422sp_2UYVY(unsigned char* YUV422sp, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x72

    int i = 0, j = 0;
    unsigned char* _Y = YUV422sp;
    unsigned char* _UV = YUV422sp + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
        }
    }
    return 0;
}

static int YUV422sp_2Gray(unsigned char* YUV422sp, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x73

    memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YUV422sp, sizeof(unsigned char) * width * height);
    return 0;
}

#endif

#if 1
/************ YVU420sp_NV21  2  ... ********/
static int YVU420sp_NV21_2RGBA(unsigned char* YVU420sp_NV21, unsigned char* RGBA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x74

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGBA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGBA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGBA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            RGBA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YVU420sp_NV21_2BGRA(unsigned char* YVU420sp_NV21, unsigned char* BGRA, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x75

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGRA[(j * width + i) * 4 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGRA[(j * width + i) * 4 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGRA[(j * width + i) * 4 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            BGRA[(j * width + i) * 4 + 3] = 255;
        }
    }
    return 0;
}

static int YVU420sp_NV21_2RGB(unsigned char* YVU420sp_NV21, unsigned char* RGB, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x76

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            RGB[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)R, 0, 255);
            RGB[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            RGB[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)B, 0, 255);
        }
    }
    return 0;
}

static int YVU420sp_NV21_2BGR(unsigned char* YVU420sp_NV21, unsigned char* BGR, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x77

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char Y, U, V;
            float R, G, B;
            Y = _Y[j * width + i];
            U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2];
            R = (float)((int)Y) + 1.140f * (float)((int)V);
            G = (float)((int)Y) - 0.394f * (float)((int)U) - 0.581f * (float)((int)V);
            B = (float)((int)Y) + 2.032f * ((float)((int)U));
            BGR[(j * width + i) * 3 + 0] = (unsigned char)AVS_Clamp((int)B, 0, 255);
            BGR[(j * width + i) * 3 + 1] = (unsigned char)AVS_Clamp((int)G, 0, 255);
            BGR[(j * width + i) * 3 + 2] = (unsigned char)AVS_Clamp((int)R, 0, 255);
        }
    }
    return 0;
}


static int YVU420sp_NV21_2_YUV420p_I420_YU12(unsigned char* YVU420sp_NV21, unsigned char* YUV420p_I420_YU12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x78

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YUV420p_I420_YU12;
    unsigned char* __U = YUV420p_I420_YU12 + width * height;
    unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
            __V[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}


static int YVU420sp_NV21_2YV12(unsigned char* YVU420sp_NV21, unsigned char* YV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x79

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YV12;
    unsigned char* __V = YV12 + width * height;
    unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
            __V[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YVU420sp_NV21_2YUV422p(unsigned char* YVU420sp_NV21, unsigned char* YUV422p, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7A

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YUV422p;
    unsigned char* __U = YUV422p + width * height;
    unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __U[j * ((width + 1) / 2) + i] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
            __V[j * ((width + 1) / 2) + i] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 0];
        }
    }
    return 0;
}


static int YVU420sp_NV21_2_YUV420sp_NV12(unsigned char* YVU420sp_NV21, unsigned char* YUV420sp_NV12, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7B

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YUV420sp_NV12;
    unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < (height + 1) / 2; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __UV[(j * ((width + 1) / 2) + i) * 2] = _VU[(j * ((width + 1) / 2) + i) * 2 + 1];
            __UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YVU420sp_NV21_2_YUV422sp(unsigned char* YVU420sp_NV21, unsigned char* YUV422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7C

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YUV422sp;
    unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < ((width + 1) / 2); i++)
        {
            __UV[(j * ((width + 1) / 2) + i) * 2] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
            __UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YVU420sp_NV21_2YVU422sp(unsigned char* YVU420sp_NV21, unsigned char* YVU422sp, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7D

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    unsigned char* __Y = YVU422sp;
    unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < (width + 1) / 2; i++)
        {
            __VU[(j * ((width + 1) / 2) + i) * 2] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
            __VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2];
        }
    }
    return 0;
}

static int YVU420sp_NV21_2YUVY(unsigned char* YVU420sp_NV21, unsigned char* YUVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7E

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            YUVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 0)
            {
                YUVY[(j * width + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
            else
            {
                YUVY[(j * width + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 0];
            }
        }
    }
    return 0;
}

static int YVU420sp_NV21_2UYVY(unsigned char* YVU420sp_NV21, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x7F

    int i = 0, j = 0;
    unsigned char* _Y = YVU420sp_NV21;
    unsigned char* _VU = YVU420sp_NV21 + width * height;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
            if (i % 2 == 1)
            {
                UYVY[(j * width + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
            }
            else
            {
                UYVY[(j * width + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 0];
            }
        }
    }
    return 0;
}

static int YVU420sp_NV21_2Gray(unsigned char* YVU420sp_NV21, unsigned char* UYVY, int width, int height) {

#undef FUNC_CODE
#define FUNC_CODE 0x80

    memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YVU420sp_NV21, sizeof(unsigned char) * width * height);
    return 0;
}

#endif

static int RGBA2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x81

    switch (oFmt)
    {
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        RGBA2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        RGBA2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        RGBA2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGBA2_YUV420P_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGBA2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGBA2YUV422P(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGBA2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGBA2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGBA2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGBA2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        RGBA2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        RGBA2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        RGBA2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        memcpy_fl(*oImage, sizeof(unsigned char) * width * height * 4, iImage, sizeof(unsigned char) * width * height * 4);
        break;
    }
    return 0;
}

static int BGRA2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x82

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        BGRA2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        BGRA2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        BGRA2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGRA2_YUV420P_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGRA2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGRA2YUV422P(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGRA2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGRA2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGRA2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGRA2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        BGRA2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        BGRA2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        BGRA2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        memcpy_fl(*oImage, sizeof(unsigned char) * width * height * 4, iImage, sizeof(unsigned char) * width * height * 4);
        break;
    }
    return 0;
}

static int RGB2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x83

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        RGB2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        RGB2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        RGB2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGB2_YUV420P_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGB2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGB2YUV422P(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGB2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGB2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        RGB2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        RGB2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        RGB2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        RGB2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        RGB2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        memcpy_fl(*oImage, sizeof(unsigned char) * width * height * 3, iImage, sizeof(unsigned char) * width * height * 3);
        break;
    }
    return 0;
}

static int BGR2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x84

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        BGR2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        BGR2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        BGR2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGR2_YUV420P_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGR2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGR2YUV422P(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGR2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGR2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        BGR2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        BGR2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        BGR2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        BGR2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        BGR2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        memcpy_fl(*oImage, sizeof(unsigned char) * width * height * 3, iImage, sizeof(unsigned char) * width * height * 3);
        break;
    }
    return 0;
}

static int YUV420P_I420_YU12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x85

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV420P_I420_YU12_2_RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV420P_I420_YU12_2_BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV420P_I420_YU12_2_RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV420P_I420_YU12_2_BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV420P_I420_YU12_2_YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420P_I420_YU12_2_YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV420P_I420_YU12_2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420P_I420_YU12_2_YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV420P_I420_YU12_2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420P_I420_YU12_2_YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YUV420P_I420_YU12_2_YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YUV420P_I420_YU12_2_UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YUV420P_I420_YU12_2_Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        break;
    }
    return 0;
}

static int YV12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x86

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YV12_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YV12_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YV12_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YV12_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YV12_2_YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YV12_2YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YV12_2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YV12_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YV12_2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YV12_2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YV12_2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YV12_2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YV12_2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        break;
    }
    return 0;
}

static int YUV422p_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x87

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV422p_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV422p_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV422p_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV422p_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV422p_2_YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV422p_2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV422p_2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV422p_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV422p_2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV422p_2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YUV422p_2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YUV422p_2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YUV422p_2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;
}

static int YUV420sp_NV12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x88

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV420sp_NV12_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV420sp_NV12_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV420sp_NV12_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV420sp_NV12_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV420sp_NV12_2_YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV420sp_NV12_2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420sp_NV12_2YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420sp_NV12_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV420sp_NV12_2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV420sp_NV12_2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YUV420sp_NV12_2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YUV420sp_NV12_2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YUV420sp_NV12_2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        break;
    }
    return 0;
}

static int YUV422sp_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x89

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV422sp_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUV422sp_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV422sp_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUV422sp_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV422sp_2_YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV422sp_2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV422sp_2YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUV422sp_2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * ((height + 1) / 2)) * 2));
        YUV422sp_2_YVU420sp_NV21(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YUV422sp_2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YUV422sp_2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YUV422sp_2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YUV422sp_2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;
}

static int YVU420sp_NV21_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0x8A

    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVU420sp_NV21_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVU420sp_NV21_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVU420sp_NV21_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVU420sp_NV21_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YVU420sp_NV21_2_YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YVU420sp_NV21_2YV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YVU420sp_NV21_2YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YVU420sp_NV21_2_YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        YVU420sp_NV21_2_YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * height * 2));
        YVU420sp_NV21_2YVU422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height * 2));
        YVU420sp_NV21_2YUVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_UYVY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 2);
        YVU420sp_NV21_2UYVY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height);
        YVU420sp_NV21_2Gray(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;
}

static int YUYV_2YUV422p(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    U = Y + width*height;
    V = U + (width*height >> 1);
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            U[j] = p[4 * j + 1];
            Y[j * 2 + 1] = p[4 * j + 2];
            V[j] = p[4 * j + 3];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);
    }
    return ret;
}




static int YUYV_2YUV422sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8C
    int ret = 0;
    unsigned char *Y,*UV;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    UV = Y + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            UV[j] = p[4 * j + 1];
            Y[j * 2 + 1] = p[4 * j + 2];
            UV[j+1] = p[4 * j + 3];
        }
        p = p + width * 2;
        Y = Y + width;
        UV = UV+ (width);
    }
    return ret;
}


// yuy2数据转换为yuv420p数据
static int YUVY_2_YUV420p(unsigned char* input, unsigned char** output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8D
    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2, *V2;
    unsigned char * p = NULL;
    p = input;
    int tem_size = (width*height) << 1;    
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = *output;
    U2 = Y2 + width*height;     
    V2 = U2 + (width*height >> 2);
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            U[j] = p[4 * j + 1];
            Y[j * 2 + 1] = p[4 * j + 2];
            V[j] = p[4 * j + 3];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        memcpy(U2, U, width >> 1);
        memcpy(V2, V, width >> 1);

        U2 = U2 + (width >> 1);
        V2 = V2 + (width >> 1);

        U = U + (width);
        V = V + (width);
    }

    memcpy(Y2, Y, width*height);

    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


static int  YUVY_2_YUV420sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8E
    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2_V2;
    unsigned char * p = NULL;
    p = input;
    int tem_size = (width*height) << 1;
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = output;
    U2_V2 = Y2 + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            U[j] = p[4 * j + 1];
            Y[j * 2 + 1] = p[4 * j + 2];
            V[j] = p[4 * j + 3];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        for (int j = 0; j < width; j+=2)
        {
            *U2_V2++ = *U++;
            *U2_V2++ = *V++;
        }

        U2_V2 = U2_V2 + (width);

        U = U + (width);
        V = V + (width);
    }
    memcpy(Y2, Y, width*height);
    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


static int YUVY_2_YVU420sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8F
    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2_V2;
    unsigned char * p = NULL;
    p = input;
    int tem_size = (width*height) << 1;
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = output;
    U2_V2 = Y2 + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            U[j] = p[4 * j + 1];
            Y[j * 2 + 1] = p[4 * j + 2];
            V[j] = p[4 * j + 3];
        }
        p = p + width * 2;
        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        for (int j = 0; j < width; j += 2)
        {
            *U2_V2++ = *V++;
            *U2_V2++ = *U++;
        }
        U2_V2 = U2_V2 + (width);
        U = U + (width);
        V = V + (width);
    }
    memcpy(Y2, Y, width*height);
    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


static int YUVY_2_YVU422sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x90   
    int ret = 0;
    unsigned char *Y, *UV;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    UV = Y + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            UV[j] = p[4 * j + 3];
            Y[j * 2 + 1] = p[4 * j + 2];
            UV[j + 1] = p[4 * j + 1];
        }
        p = p + width * 2;
        Y = Y + width;
        UV = UV + (width);
    }
    return ret;
}




static int YUVY_2_RGB_888(unsigned char* input, unsigned char** rgb, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x91   
    int ret = 0;
    unsigned char *Y, *U,*V;
    Y = input;
    U = Y + 1;
    V = U + 1;
    unsigned char* rgb1 = *rgb;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j+=4)
        {
            (*rgb1++) = *Y + 1.4075*(*V - 128);
            (*rgb1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V-128);
            (*rgb1++) = *Y + 1.779*(*U - 128);
             Y += 2;
             (*rgb1++) = *Y + 1.4075*(*V - 128);
             (*rgb1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
             (*rgb1++) = *Y + 1.779*(*U - 128);
       
            Y+=2;
            U += 4;
            V += 4;
        }
    }
    return ret;
} 

static int YUVY_2_RGBA_8888(unsigned char* input, unsigned char** rgba, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x92   
    int ret = 0;
    unsigned char *Y, *U, *V;
    Y = input;
    U = Y + 1;
    V = U + 1;
    int k, j;
    unsigned char* rgba1 = *rgba;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*rgba1++) = *Y + 1.4075*(*V - 128);
            (*rgba1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*rgba1++) = *Y + 1.779*(*U - 128);
            (*rgba1++) = 255;
            Y += 2;
            (*rgba1++) = *Y + 1.4075*(*V - 128);
            (*rgba1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*rgba1++) = *Y + 1.779*(*U - 128);
            (*rgba1++) = 255;
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}


static int YUVY_2_BGRA_8888(unsigned char* input, unsigned char** bgra, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x93
    int ret = 0;
    unsigned char *Y, *U, *V;
    Y = input;
    U = Y + 1;
    V = U + 1;
    unsigned char* bgra1 = *bgra;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*bgra1++) = *Y + 1.779*(*U - 128);
            (*bgra1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgra1++) = *Y + 1.4075*(*V - 128);
            (*bgra1++) = 255;
             Y += 2;
            (*bgra1++) = *Y + 1.779*(*U - 128);
            (*bgra1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgra1++) = *Y + 1.4075*(*V - 128);
            (*bgra1++) = 255;
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}


static int YUVY_2_BGR_888(unsigned char* input, unsigned char** bgr, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x94
    int ret = 0;
    unsigned char *Y, *U, *V;
    Y = input;
    V = Y + 1;
    U = V + 1;
    unsigned char* bgr1 = *bgr;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*bgr1++) = *Y + 1.779*(*U - 128);
            (*bgr1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgr1++) = *Y + 1.4075*(*V - 128);
            Y += 2;
            (*bgr1++) = *Y + 1.779*(*U - 128);
            (*bgr1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgr1++) = *Y + 1.4075*(*V - 128);
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}


static int  YUVY_2_GRAY(unsigned char* input, unsigned char* gray, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x95
    int ret = 0;
    unsigned char*p = input;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            *gray++ = p[4 * j];
            *gray++ = p[4 * j + 2];
        }
        p = p + width*2;
    }
    return ret;
}


static int YUVY_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0x96
    switch (oFmt)
    {
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1,width*height * 3*sizeof(unsigned char));
        YUVY_2_RGB_888(iImage, oImage, width, height);
        break;
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1,width*height * 4*sizeof(unsigned char));
        YUVY_2_RGBA_8888(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1,width*height * 4*sizeof(unsigned char));
        YUVY_2_BGRA_8888(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        //*oImage = (unsigned char*)calloc(1,width*height*3 *sizeof(unsigned char));
        *oImage = (unsigned char*)malloc(width * height * 3);
        YUVY_2_BGR_888(iImage,oImage, width, height);
        break;
    case Alva_FMT_GRAY:
        *oImage = (unsigned char*)calloc(1,width*height*sizeof(unsigned char));
        YUVY_2_GRAY(iImage,*oImage, width, height);
        break;
    case Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)calloc(1,width*height*1.5*sizeof(unsigned char));
        YUVY_2_YUV420p(iImage,oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)calloc(1,width*height*1.5*sizeof(unsigned char));
        YUVY_2_YUV420sp(iImage,*oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)calloc( 1,width*height*2 * sizeof(unsigned char));
        YUYV_2YUV422p(iImage,*oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)calloc(1,width*height * 2* sizeof(unsigned char));
        YUYV_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)calloc(1,width*height *1.5* sizeof(unsigned char));
        YUVY_2_YVU420sp(iImage,*oImage, width, height);
        break;
    case Alva_FMT_YVU_422sp:
        *oImage = (unsigned char*)calloc(1,width*height * 2*sizeof(unsigned char));
        YUVY_2_YVU422sp(iImage,*oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
    }
    return 0;
}





/*
    YVU422sp转化为YVU422p
    YVU422sp格式：YYYY.... YYYY
                  UV UV
    YVU422p格式： YYYY.... YYYY
                  UU....UU
                  VV....VV
*/
static int YVU_422sp_2YUV422P(unsigned char* yvu_422sp, unsigned char* yuv422sp, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x97
    int ret = 0;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;

    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;

    y_size = uv_size = width*height;

    p_y1 = yvu_422sp;
    p_uv = yvu_422sp + y_size;
           

    p_y2 = yuv422sp;
    p_u = yuv422sp + y_size;
    p_v = p_u + y_size / 2;

    memcpy(p_y2, p_y1, y_size);
    for (int i = 0,j=0; i < uv_size; i+=2,j++)
    {
        p_u[j] = p_uv[i];
        p_v[j] = p_uv[i + 1];
    }
    return ret;
}

/*

YVU422sp格式：YYYY.... YYYY
VU VU

yuv_420p格式：
Y通道的行数是UV通道函数的两倍(Y是U通道的行数的4倍，也是V通道的行数的4倍）
YYYY ....YYYY
YYYY ....YYYY
YYYY ....YYYY
YYYY ....YYYY
UUUU ....UUUU
VVVV ....VVVV
*/


static int YVU_422sp_2YUV420p_I420_YU12(unsigned char* yvu_422sp, unsigned char* yuv420p, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x98

    int ret = 0;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;
    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;
    y_size = uv_size = width*height;
    p_y1 = yvu_422sp;
    p_uv = yvu_422sp + y_size;
    p_y2 = yuv420p;
    p_u = yuv420p + y_size / 4;
    p_v = p_u + y_size / 4;
    memcpy(p_y2, p_y1, y_size);
    for (int i =0,k =0; i < height/2; i +=2, k++)
    {
        for (int j = 0; j < width/2;j++)
        {
            p_v[k*width + j] = p_uv[i*width + j];
            p_u[k*width + j] = p_uv[i*width + j + 1];
            p_v[k*width + j + width/2]= p_uv[(i+1)*width + j];
            p_u[k*width + j + width / 2+1]= p_uv[(i+1)*width + j + 1];
        }
    }
    return ret;
}




static int YVU_422sp_2YUV422sp(unsigned char* yvu_422sp, unsigned char* yuv_422sp, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x99

    int ret = 0;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_vu;
    unsigned char* p_y2;
    unsigned char* p_uv;
    y_size = uv_size = width*height;
    p_y1 = yvu_422sp;
    p_vu = yvu_422sp + y_size;
    p_y2 = yuv_422sp;
    p_uv = yuv_422sp + y_size;
    memcpy(p_y2, p_y1, y_size);
    for (int i = 0, j = 0; i < uv_size; i += 2, j+=2)
    {
        p_uv[j] = p_vu[i+1];
        p_uv[j+1] = p_uv[i];
    }
    return ret;
}



/*
yuv_420sp格式：
Y通道的行数是UV通道函数的两倍
YYYY ....YYYY
YYYY ....YYYY
UVUV ....UVUV
*/

static int YVU_422sp_2YUV420sp_NV12(unsigned char* yvu_422sp, unsigned char* yuv420sp_nv12, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9A

    int ret = 0;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;
    unsigned char* p_y2;
    unsigned char* p_uv2;
    y_size = uv_size = width*height;
    p_y1 = yvu_422sp;
    p_uv = yvu_422sp + y_size;
    p_y2 = yuv420sp_nv12;
    p_uv2 = yuv420sp_nv12 + y_size;
    memcpy(p_y2, p_y1, y_size); 
    for (int i = 0,k=0; i < height; i+=2,k++)
    {
        for (int j = 0; j < width; j += 2)
        {
            p_uv2[k*width + j] = p_uv[i*width + j];
            p_uv2[k*width + j + 1] = p_uv[i*width + j + 1];
        }
    }
    return ret;
}


/*
yuv_420sp格式：
Y通道的行数是UV通道函数的两倍
YYYY ....YYYY
YYYY ....YYYY
VUVU ....VUVU
*/

static int YVU_422sp_2YVU_420sp(unsigned char* yvu_422sp, unsigned char* yvu420sp, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9B

    int ret = 0;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;
    unsigned char* p_y2;
    unsigned char* p_uv2;
    y_size = uv_size = width*height;
    p_y1 = yvu_422sp;
    p_uv = yvu_422sp + y_size;
    p_y2 = yvu420sp;
    p_uv2 = yvu420sp + y_size;

    memcpy(p_y2, p_y1, y_size);
    for (int i = 0, k = 0; i < height; i += 2, k++)
    {
        for (int j = 0; j < width; j += 2)
        {
            p_uv2[k*width + j] = p_uv[i*width + j+1];
            p_uv2[k*width + j + 1] = p_uv[i*width + j];
        }
    }
    return ret;
}


static int YVU_422sp_2RGBA(unsigned char* yvu_422sp, unsigned char** rgba, int  width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9C

    int ret = 0;
    unsigned char* y = yvu_422sp;
    unsigned char* vu = y + width*height;
    unsigned char* rgba1 = *rgba;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j += 4)
        {
            *(rgba1++) = (unsigned char)((*y) + 1.4075f*(*vu - 128));
            *(rgba1++) = (unsigned char)((*y) - 0.3455f*(*(vu + 1) - 128) - 0.7169*(*vu - 128));
            *(rgba1++) = (unsigned char)((*y) + 1.779f*(*(vu + 1)-128));
            *(rgba1++) = 255;
            y++;
            vu += 2;
            *(rgba1++) = (unsigned char)((*y) + 1.4075f*(*vu - 128));
            *(rgba1++) = (unsigned char)((*y) - 0.3455f*(*(vu + 1) - 128) - 0.7169*(*vu - 128));
            *(rgba1++) = (unsigned char)((*y) + 1.779f*(*(vu + 1) - 128));
            *(rgba1++) = 255;
            y++;
            vu += 2;
        }
    }
    return ret;
}


static int YVU_422sp_2RGB(unsigned char* yvu_422sp, unsigned char** rgb, int  width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9D

    int ret = 0;
    unsigned char* y = yvu_422sp;
    unsigned char* vu = y + width*height;
    unsigned char* rgb1 = *rgb;;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j += 4)
        {
            *(rgb1++) = (*y) + 1.4075*(*vu - 128);
            *(rgb1++) = (*y) - 0.3455*(*(vu + 1) - 128) - 0.7169*(*vu - 128);
            *(rgb1++) = (*y) + 1.779*(*(vu + 1) - 128);
            y++;
            vu += 2;
            *(rgb1++) = (*y) + 1.4075*(*vu - 128);
            *(rgb1++) = (*y) - 0.3455*(*(vu + 1) - 128) - 0.7169*(*vu - 128);
            *(rgb1++) = (*y) + 1.779*(*(vu + 1) - 128);
            y++;
            vu += 2;
        }
    }
    return ret;
}

static int YVU_422sp_2BGRA(unsigned char* yvu_422sp, unsigned char** bgra, int  width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9E

    int ret = 0;
    unsigned char* y = yvu_422sp;
    unsigned char* vu = y + width*height;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j += 4)
        {
            *(*bgra++) = (*y) + 1.779*(*(vu + 1) - 128);
            *(*bgra++) = (*y) - 0.3455*(*(vu + 1) - 128) - 0.7169*(*vu - 128);
            *(*bgra++) = (*y) + 1.4075*(*vu - 128);
            *(*bgra++) = 255;
            y++;
            vu += 2;
            *(*bgra++) = (*y) + 1.779*(*(vu + 1) - 128);
            *(*bgra++) = (*y) - 0.3455*(*(vu + 1) - 128) - 0.7169*(*vu - 128);
            *(*bgra++) = (*y) + 1.4075*(*vu - 128);
            *(*bgra++) = 255;
            y++;
            vu += 2;
        }
    }
    return ret;
}


static int YVU_422sp_2BGR(unsigned char* yvu_422sp, unsigned char** bgr, int  width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x9F

    int ret = 0;
    unsigned char* y = yvu_422sp;
    unsigned char* vu = y + width*height;
    unsigned char* tempbgr = *bgr;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j += 4)
        {
            *(tempbgr++) = (float)((int)(*y)) + 1.779*((int)(*(vu + 1)) - 128);
            *(tempbgr++) = (float)((int)(*y)) - 0.3455*((int)(*(vu + 1)) - 128 - 0.7169*(*vu - 128));
            *(tempbgr++) = (float)((int)(*y)) + 1.4075*((int)(*vu) - 128);
            y++;
            vu += 2;
            *(tempbgr++) = (float)((int)(*y)) + 1.779*(float)(((int)(*y)) - 128);
            *(tempbgr++) = (float)((int)(*y)) - 0.3455*((int)(*(vu + 1)) - 128 - 0.7169*(*vu - 128));
            *(tempbgr++) = (float)((int)(*y)) + 1.4075*((int)(*vu) - 128);
            y++;
            vu += 2;
        }
    }
    return ret;
}


static int YVU422sp_2GRAY(unsigned char* yvu_422sp, unsigned char** gray, int  width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA0

    int ret = 0;
    memcpy(gray, yvu_422sp, width*height);
    return ret;
}


static int YVU_422sp_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA1

    int ret = 0;
    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 4);
        YVU_422sp_2RGBA(iImage, oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 3);
        YVU_422sp_2RGB(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 3);
        YVU_422sp_2BGR(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 4);
        YVU_422sp_2BGRA(iImage, oImage, width, height);
        break;
    case  Alva_FMT_GRAY:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
        YVU422sp_2GRAY(iImage, oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);
        YVU_422sp_2YUV422P(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        YVU_422sp_2YUV420sp_NV12(iImage, *oImage, width, height);
        break;
    case  Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        YVU_422sp_2YUV420p_I420_YU12(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);
        YVU_422sp_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        YVU_422sp_2YVU_420sp(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
    }
    return ret;
}





static int UYVY_2YUV422sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA2

    int ret = 0;
    unsigned char *Y, *UV;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    UV = Y + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            UV[j]= p[4 * j];
            Y[j * 2] = p[4 * j+1];
            UV[j + 1] = p[4 * j + 2];
            Y[j+3]= p[4 * j + 3];
        }
        p = p + width * 2;
        Y = Y + width;
        UV = UV + (width);
    }
    return ret;
}


// yuy2数据转换为yuv420p数据
static int UYVY_2_YUV420p(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA3

    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2, *V2;
    unsigned char * p = NULL;
    int tem_size = (width*height) << 1;
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = output;
    U2 = Y2 + width*height;
    V2 = U2 + (width*height >> 2);
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            U[j] = p[4 * j];
            Y[j * 2] = p[4 * j + 1];
            V[j] = p[4 * j + 2];
            Y[j * 2 + 1] = p[4 * j + 3];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        memcpy(U2, U, width >> 1);
        memcpy(V2, V, width >> 1);

        U2 = U2 + (width >> 1);
        V2 = V2 + (width >> 1);

        U = U + (width);
        V = V + (width);
    }

    memcpy(Y2, Y, width*height);

    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


// yuy2数据转换为yuv420sp数据
static int  UYVY_2_YUV420sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA4

    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2_V2;
    unsigned char * p = NULL;
    p = input;
    int tem_size = (width*height) << 1;
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = output;
    U2_V2 = Y2 + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j+1];
            U[j] = p[4 * j];
            Y[j * 2 + 1] = p[4 * j + 3];
            V[j] = p[4 * j +2];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        for (int j = 0; j < width; j += 2)
        {
            *U2_V2++ = *U++;
            *U2_V2++ = *V++;
        }
        U2_V2 = U2_V2 + (width);

        U = U + (width);
        V = V + (width);
    }
    memcpy(Y2, Y, width*height);
    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


static int UYVY_2_YVU420sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA5

    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2_V2;
    unsigned char * p = NULL;
    p = input;
    int tem_size = (width*height) << 1;
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);
    Y2 = output;
    U2_V2 = Y2 + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j+1];
            U[j] = p[4 * j];
            Y[j * 2 + 1] = p[4 * j + 3];
            V[j] = p[4 * j + 2];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);

    }
    //经过for循环后变为planar形式
    //复位
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);

    int l;
    for (l = 0; l < height / 2; ++l)
    {
        for (int j = 0; j < width; j += 2)
        {
            *U2_V2++ = *V++;
            *U2_V2++ = *U++;
        }

        U2_V2 = U2_V2 + (width);

        U = U + (width);
        V = V + (width);
    }
    memcpy(Y2, Y, width*height);
    if (tem_buf)
    {
        free(tem_buf);
        tem_buf = NULL;
    }
    return ret;
}


static int UYVY_2YUV422p(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA6

    int ret = 0;
    unsigned char *Y, *U, *V;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    U = Y + width*height;
    V = U + (width*height >> 1);
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j+1];
            U[j] = p[4 * j ];
            Y[j * 2 + 1] = p[4 * j + 3];
            V[j] = p[4 * j + 2];
        }
        p = p + width * 2;

        Y = Y + width;
        U = U + (width >> 1);
        V = V + (width >> 1);
    }
    return ret;
}



static int UYVY_2_YVU422sp(unsigned char* input, unsigned char* output, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA7

    int ret = 0;
    unsigned char *Y, *UV;
    unsigned char * p = NULL;
    p = input;
    Y = output;
    UV = Y + width*height;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            Y[j * 2] = p[4 * j+1];
            UV[j] = p[4 * j];
            Y[j * 2 + 1] = p[4 * j + 3];
            UV[j + 1] = p[4 * j + 2];
        }
        p = p + width * 2;

        Y = Y + width;
        UV = UV + (width);
    }
    return ret;
}




static int UYVY_2_RGB_888(unsigned char* input, unsigned char** rgb, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA8
    int ret = 0;
    unsigned char *Y, *U, *V;
    U = input;
    Y = U + 1;
    V = Y + 1;
    unsigned char* rgb1 = *rgb;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*rgb1++) = *Y + 1.4075*(*V - 128);
            (*rgb1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*rgb1++) = *Y + 1.779*(*U - 128);
            Y += 2;
            (*rgb1++) = *Y + 1.4075*(*V - 128);
            (*rgb1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*rgb1++) = *Y + 1.779*(*U - 128);
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}

static int UYVY_2_RGBA_8888(unsigned char* input, unsigned char** rgba, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xA9

    int ret = 0;
    unsigned char *Y, *U, *V;
    U = input;
    Y = U + 1;
    V = Y + 1;
    unsigned char* rbga1 = *rgba;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*rbga1++) = *Y + 1.4075*(*V - 128);
            (*rbga1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*rbga1++) = *Y + 1.779*(*U - 128);
            (*rbga1++) = 255;
            Y += 2;
            (*rbga1++) = *Y + 1.4075*(*Y - 128);
            (*rbga1++) = *Y + 1.4075*(*Y - 128);
            (*rbga1++) = *Y + 1.4075*(*Y - 128);
            (*rbga1++) = 255;
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}


static int UYVY_2_BGRA_8888(unsigned char* input, unsigned char** bgra, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xAA

    int ret = 0;
    unsigned char *Y, *U, *V;
    U = input;
    Y = U + 1;
    V = Y + 1;
    unsigned char* bgra1 = *bgra;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*bgra1++) = *Y + 1.779*(*U - 128);
            (*bgra1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgra1++) = *Y + 1.4075*(*V - 128);
            (*bgra1++) = 255;
            Y += 2;
            (*bgra1++) = *Y + 1.779*(*U - 128);
            (*bgra1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgra1++) = *Y + 1.4075*(*V - 128);
            (*bgra1++) = 255;
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}


static int UYVY_2_BGR_888(unsigned char* input, unsigned char** bgr, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xAB

    int ret = 0;
    unsigned char *Y, *U, *V;
    U = input;
    Y = U + 1;
    V = Y + 1;
    unsigned char* bgr1 = *bgr;
    int k, j;
    for (k = 0; k < height; k++)
    {
        for (j = 0; j < width; j += 4)
        {
            (*bgr1++) = *Y + 1.779*(*U - 128);
            (*bgr1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgr1++) = *Y + 1.4075*(*V - 128);
            Y += 2;
            (*bgr1++) = *Y + 1.779*(*U - 128);
            (*bgr1++) = *Y - 0.3455*(*U - 128) - 0.7169*(*V - 128);
            (*bgr1++) = *Y + 1.4075*(*V - 128);
            Y += 2;
            U += 4;
            V += 4;
        }
    }
    return ret;
}



static int  UYVY_2_GRAY(unsigned char* input, unsigned char* gray, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0xAC
    int ret = 0;
    unsigned char*p = input+1;
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (width >> 1); ++j)
        {
            *gray++ = p[4 * j];
            *gray++ = p[4 * j + 2];
        }
        p = p + width * 2;
    }
    return ret;
}



static int UYVY_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0xAD

    int ret = 0;
    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 4);
        UYVY_2_RGBA_8888(iImage, oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 3);
        UYVY_2_RGB_888(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 3);
        UYVY_2_BGR_888(iImage, oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 4);
        UYVY_2_BGRA_8888(iImage, oImage, width, height);
        break;
    case  Alva_FMT_GRAY:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
        UYVY_2_GRAY(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422p:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);
        UYVY_2YUV422p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_420sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        UYVY_2_YUV420sp(iImage, *oImage, width, height);
        break;
    case  Alva_FMT_YUV_420p:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        UYVY_2_YUV420p(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YUV_422sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);
        UYVY_2YUV422sp(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YVU_420sp:
        *oImage = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 1.5);
        UYVY_2_YVU422sp(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
    }
    return ret;
}


int ImageTypeConverter(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt iFmt, alvafmt oFmt) {

#undef FUNC_CODE
#define FUNC_CODE 0xAE

    switch (iFmt)
    {
    case Alva_FMT_RGBA_8888:
        RGBA2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_BGRA_8888:
        BGRA2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_RGB_888:
        RGB2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_BGR_888:
        BGR2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YUV_420p:
        YUV420P_I420_YU12_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YV12:
        YV12_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YUV_422p:
        YUV422p_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YUV_420sp:
        YUV420sp_NV12_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YUV_422sp:
        YUV422sp_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YVU_420sp:
        YVU420sp_NV21_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YVU_422sp:
        YVU_422sp_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_YUVY:
        YUVY_2(iImage, oImage, width, height, oFmt);
        break;
    case Alva_FMT_UYVY:
        UYVY_2(iImage, oImage, width, height, oFmt);
        break;
    default://Alva_FMT_Gray
        break;
    }
    return 0;
}
