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
static int RGBA2BGRA(unsigned char* RGBA, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x01

	int i = 0;
	for(i = 0; i < width * height * 4; i += 4)
	{
		BGRA[i    ] = RGBA[i + 2];
		BGRA[i + 1] = RGBA[i + 1];
		BGRA[i + 2] = RGBA[i    ];
		BGRA[i + 3] = RGBA[i + 3];
	}
	return 0;
}

static int RGBA2RGB(unsigned char* RGBA, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x02

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
	{
		RGB[j    ] = RGBA[i    ];
		RGB[j + 1] = RGBA[i + 1];
		RGB[j + 2] = RGBA[i + 2];
	}
	return 0;
}

static int RGBA2BGR(unsigned char* RGBA, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x03

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
	{
		BGR[j    ] = RGBA[i + 2];
		BGR[j + 1] = RGBA[i + 1];
		BGR[j + 2] = RGBA[i    ];
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
static int RGBA2_YUV420P_I420_YU12(unsigned char* RGBA, unsigned char* YUV420P_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x04

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGBA2YV12(unsigned char* RGBA, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x05

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGBA2YUV422P(unsigned char* RGBA, unsigned char* YUV422P, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x06

	int i = 0, j = 0;
	unsigned char* _Y = YUV422P;
	unsigned char* _U = YUV422P + width * height;
	unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGBA2_YUV420sp_NV12(unsigned char* RGBA, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x07

	int i = 0, j = 0;
	unsigned char* _Y  = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int RGBA2YUV422sp(unsigned char* RGBA, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x08

	int i = 0, j = 0;
	unsigned char* _Y  = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int RGBA2_YVU420sp_NV21(unsigned char* RGBA, unsigned char* YVU420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x09

	int i = 0, j = 0;
	unsigned char* _Y  = YVU420sp_NV12;
	unsigned char* _VU = YVU420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int RGBA2YVU422sp(unsigned char* RGBA, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0A

	int i = 0, j = 0;
	unsigned char* _Y  = YVU422sp;
	unsigned char* _VU = YVU422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4    ];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int RGBA2YUVY(unsigned char* RGBA, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0B

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGBA2UYVY(unsigned char* RGBA, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0C

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGBA[(j * width + i) * 4];
			G = RGBA[(j * width + i) * 4 + 1];
			B = RGBA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 1)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
			}
		}
	}
	return 0;
}

static int RGBA2Gray(unsigned char* RGBA, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0D

	int i = 0;
#pragma omp parallel for private(i) shared(width, height, Gray) 
	for(i = 0; i < width * height * 4; i += 4)
	{
		int R = 0, G = 0, B = 0;
		float Y = 0.0f;
		R = RGBA[i];
		G = RGBA[i + 1];
		B = RGBA[i + 2];
		Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
		Gray[i / 4] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
	}
	return 0;	
}

#endif

#if 1
/******************BGRA   2    ... ****************/
static int BGRA2RGBA(unsigned char* BGRA, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0E

	int i = 0;
	for(i = 0; i < width * height * 4; i += 4)
	{
		RGBA[i    ] = BGRA[i + 2];
		RGBA[i + 1] = BGRA[i + 1];
		RGBA[i + 2] = BGRA[i    ];
		RGBA[i + 3] = BGRA[i + 3];
	}
	return 0;
}

static int BGRA2RGB(unsigned char* BGRA, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x0F

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
	{
		RGB[j    ] = BGRA[i + 2];
		RGB[j + 1] = BGRA[i + 1];
		RGB[j + 2] = BGRA[i    ];
	}
	return 0;
}

static int BGRA2BGR(unsigned char* BGRA, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x10

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 4; i += 4, j += 3)
	{
		BGR[j    ] = BGRA[i    ];
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
static int BGRA2_YUV420P_I420_YU12(unsigned char* BGRA, unsigned char* YUV420P_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x11

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGRA2YV12(unsigned char* BGRA, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x12

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGRA2YUV422P(unsigned char* BGRA, unsigned char* YUV422P, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x13

	int i = 0, j = 0;
	unsigned char* _Y = YUV422P;
	unsigned char* _U = YUV422P + width * height;
	unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGRA2_YUV420sp_NV12(unsigned char* BGRA, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x14

	int i = 0, j = 0;
	unsigned char* _Y  = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int BGRA2YUV422sp(unsigned char* BGRA, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x15

	int i = 0, j = 0;
	unsigned char* _Y  = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int BGRA2_YVU420sp_NV21(unsigned char* BGRA, unsigned char* YVU420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x16

	int i = 0, j = 0;
	unsigned char* _Y  = YVU420sp_NV12;
	unsigned char* _VU = YVU420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int BGRA2YVU422sp(unsigned char* BGRA, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x17

	int i = 0, j = 0;
	unsigned char* _Y  = YVU422sp;
	unsigned char* _VU = YVU422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4    ];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int BGRA2YUVY(unsigned char* BGRA, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x18

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGRA2UYVY(unsigned char* BGRA, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x19

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGRA[(j * width + i) * 4];
			G = BGRA[(j * width + i) * 4 + 1];
			R = BGRA[(j * width + i) * 4 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 1)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
			}
		}
	}
	return 0;
}

static int BGRA2Gray(unsigned char* BGRA, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1A

	int i = 0;
	for(i = 0; i < width * height * 4; i += 4)
	{
		int R = 0, G = 0, B = 0;
		float Y = 0.0f;
		B = BGRA[i];
		G = BGRA[i + 1];
		R = BGRA[i + 2];
		Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
		Gray[i / 4] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
	}
	return 0;
}
#endif

#if 1
/***************** RGB 2 ... ********/
static int RGB2RGBA(unsigned char* RGB, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1B

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
	{
		RGBA[j    ] = RGB[i    ];
		RGBA[j + 1] = RGB[i + 1];
		RGBA[j + 2] = RGB[i + 2];
		RGBA[j + 3] = 255;
	}
	return 0;
}

static int RGB2BGRA(unsigned char* RGB, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1C

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
	{
		BGRA[j    ] = RGB[i + 2];
		BGRA[j + 1] = RGB[i + 1];
		BGRA[j + 2] = RGB[i    ];
		BGRA[j + 3] = 255;
	}
	return 0;
}

static int RGB2BGR(unsigned char* RGB, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1D

	int i = 0;
	for(i = 0; i < width * height * 3; i += 3)
	{
		BGR[i    ] = RGB[i + 2];
		BGR[i + 1] = RGB[i + 1];
		BGR[i + 2] = RGB[i    ];
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
static int RGB2_YUV420P_I420_YU12(unsigned char* RGB, unsigned char* YUV420P_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1E

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGB2YV12(unsigned char* RGB, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x1F

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGB2YUV422P(unsigned char* RGB, unsigned char* YUV422P, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x20

	int i = 0, j = 0;
	unsigned char* _Y = YUV422P;
	unsigned char* _U = YUV422P + width * height;
	unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGB2_YUV420sp_NV12(unsigned char* RGB, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x21

	int i = 0, j = 0;
	unsigned char* _Y  = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int RGB2YUV422sp(unsigned char* RGB, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x22

	int i = 0, j = 0;
	unsigned char* _Y  = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int RGB2_YVU420sp_NV21(unsigned char* RGB, unsigned char* YVU420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x23

	int i = 0, j = 0;
	unsigned char* _Y  = YVU420sp_NV12;
	unsigned char* _VU = YVU420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int RGB2YVU422sp(unsigned char* RGB, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x24

	int i = 0, j = 0;
	unsigned char* _Y  = YVU422sp;
	unsigned char* _VU = YVU422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int RGB2YUVY(unsigned char* RGB, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x25

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3    ];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int RGB2UYVY(unsigned char* RGB, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x26

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			R = RGB[(j * width + i) * 3];
			G = RGB[(j * width + i) * 3 + 1];
			B = RGB[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 1)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
			}
		}
	}
	return 0;
}

static int RGB2Gray(unsigned char* RGB, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x27

	int i = 0;
	for(i = 0; i < width * height * 3; i += 3)
	{
		int R = 0, G = 0, B = 0;
		float Y = 0.0f;
		R = RGB[i];
		G = RGB[i + 1];
		B = RGB[i + 2];
		Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
		Gray[i / 3] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
	}
	return 0;
}
#endif

#if 1
/****************  BGR   2   ...  */
static int BGR2RGBA(unsigned char* BGR, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x28

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
	{
		RGBA[j    ] = BGR[i + 2];
		RGBA[j + 1] = BGR[i + 1];
		RGBA[j + 2] = BGR[i    ];
		RGBA[j + 3] = 255;
	}
	return 0;
}

static int BGR2BGRA(unsigned char* BGR, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x29

	int i = 0, j = 0;
	for(i = 0, j = 0; i < width * height * 3; i += 3, j += 4)
	{
		BGRA[j    ] = BGR[i    ];
		BGRA[j + 1] = BGR[i + 1];
		BGRA[j + 2] = BGR[i + 2];
		BGRA[j + 3] = 255;
	}
	return 0;
}

static int BGR2RGB(unsigned char* BGR, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2A

	int i = 0;
	for(i = 0; i < width * height * 3; i += 3)
	{
		RGB[i    ] = BGR[i + 2];
		RGB[i + 1] = BGR[i + 1];
		RGB[i + 2] = BGR[i    ];
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
static int BGR2_YUV420P_I420_YU12(unsigned char* BGR, unsigned char* YUV420P_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2B

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGR2YV12(unsigned char* BGR, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2C

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(j % 2 == 0 && i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGR2YUV422P(unsigned char* BGR, unsigned char* YUV422P, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2D

	int i = 0, j = 0;
	unsigned char* _Y = YUV422P;
	unsigned char* _U = YUV422P + width * height;
	unsigned char* _V = YUV422P + width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGR2_YUV420sp_NV12(unsigned char* BGR, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2E

	int i = 0, j = 0;
	unsigned char* _Y  = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int BGR2YUV422sp(unsigned char* BGR, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x2F

	int i = 0, j = 0;
	unsigned char* _Y  = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_UV[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)U, 0, 255);
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
static int BGR2_YVU420sp_NV21(unsigned char* BGR, unsigned char* YVU420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x30

	int i = 0, j = 0;
	unsigned char* _Y  = YVU420sp_NV12;
	unsigned char* _VU = YVU420sp_NV12 + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0 && j % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j / 2 * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int BGR2YVU422sp(unsigned char* BGR, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x31

	int i = 0, j = 0;
	unsigned char* _Y  = YVU422sp;
	unsigned char* _VU = YVU422sp + width * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3    ];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			_Y[j * width + i] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				_VU[(j * ((width + 1) / 2) + i / 2) * 2    ] = (unsigned char)AVS_Clamp((int)V, 0, 255);
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
static int BGR2YUVY(unsigned char* BGR, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x32

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i)    ];
			G = BGR[(j * width + i) + 1];
			R = BGR[(j * width + i) + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 0)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
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
static int BGR2UYVY(unsigned char* BGR, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x33

	int i = 0, j = 0;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			int R = 0, G = 0, B = 0;
			float Y = 0.0f, U = 0.0f, V = 0.0f;
			B = BGR[(j * width + i) * 3];
			G = BGR[(j * width + i) * 3 + 1];
			R = BGR[(j * width + i) * 3 + 2];
			Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
			YUVY[(j * width + i) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
			if(i % 2 == 1)
			{
				U = -0.147f * (float)R - 0.289f * (float)G + 0.436f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)U, 0, 255);
			}else
			{
				V =  0.615f * (float)R - 0.515f * (float)G - 0.100f * (float)B;
				YUVY[(j * width + i) * 2 + 1] = (unsigned char)AVS_Clamp((int)V, 0, 255);
			}
		}
	}
	return 0;
}

static int BGR2Gray(unsigned char* BGR, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x34

	int i = 0;
	for(i = 0; i < width * height * 3; i += 3)
	{
		int R = 0, G = 0, B = 0;
		float Y = 0.0f;
		B = BGR[i];
		G = BGR[i + 1];
		R = BGR[i + 2];
		Y =  0.299f * (float)R + 0.587f * (float)G + 0.114f * (float)B;
		Gray[(i / 3) * 2 + 0] = (unsigned char)AVS_Clamp((int)Y, 0, 255);
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
static int YUV420P_I420_YU12_2_RGBA(unsigned char* YUV420P_I420_YU12, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x35

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420P_I420_YU12_2_BGRA(unsigned char* YUV420P_I420_YU12, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x36

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420P_I420_YU12_2_RGB(unsigned char* YUV420P_I420_YU12, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x37

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420P_I420_YU12_2_BGR(unsigned char* YUV420P_I420_YU12, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x38

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

/****YV12存储格式****
*	Y1	Y2	Y3	Y4	Y5	Y6	Y7	Y8
*	Y9	Y10	Y11	Y12	Y13	Y14	Y15	Y16
*	Y17	Y18	Y19	Y20	Y21	Y22	Y23	Y24
*	Y25	Y26	Y27	Y28	Y29	Y30	Y31	Y32
*	V1	V2	V3	V4	V5	V6	V7	V8
*	U1	U2	U3	U4	U5	U6	U7	U8
******************************************/
static int YUV420P_I420_YU12_2_YV12(unsigned char* YUV420P_I420_YU12, unsigned char* YV12, int width, int height){

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
static int YUV420P_I420_YU12_2_YUV422p(unsigned char* YUV420P_I420_YU12, unsigned char* YUV422p, int width, int height){

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

	for(i = 0; i < height; i ++)
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
static int YUV420P_I420_YU12_2_YUV420sp_NV12(unsigned char* YUV420P_I420_YU12, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x3B

	int i = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YUV420sp_NV12;
	unsigned char* __UV = YUV420sp_NV12 + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(i = 0; i < ((width + 1) / 2) * ((height + 1) / 2); i ++)
	{
		__UV[i * 2    ] = _U[i];
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
static int YUV420P_I420_YU12_2_YUV422sp(unsigned char* YUV420P_I420_YU12, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x3C

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YUV422sp;
	unsigned char* __UV = YUV422sp + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__UV[(j * ((width + 1) / 2) + i) * 2    ] = _U[(j / 2) * ((width + 1) / 2) + i];
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
static int YUV420P_I420_YU12_2_YVU420sp_NV21(unsigned char* YUV420P_I420_YU12, unsigned char* YVU420sp_NV21, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x3D

	int i = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YVU420sp_NV21;
	unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(i = 0; i < ((width + 1) / 2) * (height + 1) / 2; i ++)
	{
		__VU[i * 2    ] = _V[i];
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
static int YUV420P_I420_YU12_2_YVU422sp(unsigned char* YUV420P_I420_YU12, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x3E

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _V[(j / 2) * ((width + 1) / 2) + i];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

/****YUVY存储格式****
*	Y1U1	Y2V2	Y3U3	Y4V4	Y5U5	Y6V6	Y7U7	Y8V8
*	Y9U9	Y10V10	Y11U11	Y12V12	Y13U13	Y14V14  Y15U15	Y16V16
******************************************/
static int YUV420P_I420_YU12_2_YUVY(unsigned char* YUV420P_I420_YU12, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x3F

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}else
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
static int YUV420P_I420_YU12_2_UYVY(unsigned char* YUV420P_I420_YU12, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x40

	int i = 0, j = 0;
	unsigned char* _Y = YUV420P_I420_YU12;
	unsigned char* _U = YUV420P_I420_YU12 + width * height;
	unsigned char* _V = YUV420P_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
            UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
                UYVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}else
			{
                UYVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}
		}
	}
	return 0;
}

static int YUV420P_I420_YU12_2_Gray(unsigned char* YUV420P_I420_YU12, unsigned char* Gray, int width, int height){

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
static int YV12_2RGBA(unsigned char* YV12, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x42

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YV12_2BGRA(unsigned char* YV12, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x43

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YV12_2RGB(unsigned char* YV12, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x44

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YV12_2BGR(unsigned char* YV12, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x45

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);
	
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YV12_2_YUV420p_I420_YU12(unsigned char* YV12, unsigned char* YUV420P_I420_YU12, int width, int height){

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

static int YV12_2YUV422p(unsigned char* YV12, unsigned char* YUV422p, int width, int height){

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

	for(i = 0; i < height; i ++)
	{
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[(i / 2) * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
	}
	return 0;
}

static int YV12_2_YUV420sp_NV12(unsigned char* YV12, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x48

	int i = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YUV420sp_NV12;
	unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(i = 0; i < ((width + 1) / 2) * ((height + 1) / 2); i ++)
	{
		__UV[i * 2    ] = _U[i];
		__UV[i * 2 + 1] = _V[i];
	}
	return 0;
}

static int YV12_2YUV422sp(unsigned char* YV12, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x49

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YUV422sp;
	unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__UV[(j * ((width + 1) / 2) + i) * 2    ] = _U[(j / 2) * ((width + 1) / 2) + i];
			__UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + i];
		}
	}

	return 0;
}

static int YV12_2_YVU420sp_NV21(unsigned char* YV12, unsigned char* YVU420sp_NV21, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4A

	int i = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YVU420sp_NV21;
	unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(i = 0; i < ((width + 1) / 2) * (height + 1) / 2; i ++)
	{
		__VU[i * 2    ] = _V[i];
		__VU[i * 2 + 1] = _U[i];
	}
	return 0;
}

static int YV12_2YVU422sp(unsigned char* YV12, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4B

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _V[(j / 2) * ((width + 1) / 2) + i];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

static int YV12_2YUVY(unsigned char* YV12, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4C

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}else
			{
				YUVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}
		}
	}

	return 0;
}

static int YV12_2UYVY(unsigned char* YV12, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4D

	int i = 0, j = 0;
	unsigned char* _Y = YV12;
	unsigned char* _V = YV12 + width * height;
	unsigned char* _U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
				UYVY[(j * width + i) * 2 + 1] = _U[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}else
			{
				UYVY[(j * width + i) * 2 + 1] = _V[(j / 2) * ((width + 1) / 2) + (i / 2)];
			}
		}
	}
	return 0;
}

static int YV12_2Gray(unsigned char* YV12, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4E

    memcpy_fl(Gray, sizeof(unsigned char) * width * height, YV12, sizeof(unsigned char) * width * height);
	return 0;
}

#endif

#if 1
/****************** YUV422p  2  ...  *******/
static int YUV422p_2RGBA(unsigned char* YUV422p, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x4F

	int i = 0, j =  0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p +  width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422p_2BGRA(unsigned char* YUV422p, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x50

	int i = 0, j =  0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p +  width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422p_2RGB(unsigned char* YUV422p, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x51

	int i = 0, j =  0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p +  width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422p_2BGR(unsigned char* YUV422p, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x52

	int i = 0, j =  0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p +  width * height + ((width + 1) / 2) * height;
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422p_2_YUV420p_I420_YU12(unsigned char* YUV422p, unsigned char* YUV420P_I420_YU12, int width, int height){

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

	for(i = 0; i < (height + 1) / 2; i ++)
	{
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
	}
	return 0;
}

static int YUV422p_2YV12(unsigned char* YUV422p, unsigned char* YV12, int width, int height){

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

	for(i = 0; i < (height + 1) / 2; i ++)
	{
        memcpy_fl(&(__U[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_U[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
        memcpy_fl(&(__V[i * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2), &(_V[i * 2 * ((width + 1) / 2)]), sizeof(unsigned char) * ((width + 1) / 2));
	}
	return 0;
}

static int YUV422p_2_YUV420sp_NV12(unsigned char* YUV422p, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x54

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	unsigned char* __Y = YUV420sp_NV12;
	unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__UV[(j * ((width + 1) / 2) + i) * 2 + 0] = _U[(j * 2) * ((width + 1) / 2) + i];
			__UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _V[(j * 2) * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

static int YUV422p_2YUV422sp(unsigned char* YUV422p, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x55

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	unsigned char* __Y = YUV422sp;
	unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__UV[(j * ((width + 1) / 2)) * 2 + 0] = _U[j * ((width + 1) / 2) + i];
			__UV[(j * ((width + 1) / 2)) * 2 + 1] = _V[j * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

static int YUV422p_2_YVU420sp_NV21(unsigned char* YUV422p, unsigned char* YVU420sp_NV21, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x56

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	unsigned char* __Y = YVU420sp_NV21;
	unsigned char* __VU = YVU420sp_NV21 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2)) * 2 + 0] = _V[j * 2 * ((width + 1) / 2) + i];
			__VU[(j * ((width + 1) / 2)) * 2 + 1] = _U[j * 2 * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

static int YUV422p_2YVU422sp(unsigned char* YUV422p, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x57

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2)) * 2 + 0] = _V[j * ((width + 1) / 2) + i];
			__VU[(j * ((width + 1) / 2)) * 2 + 1] = _U[j * ((width + 1) / 2) + i];
		}
	}
	return 0;
}

static int YUV422p_2YUVY(unsigned char* YUV422p, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x58

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _U[j * ((width + 1) / 2) + i / 2];
			}else
			{
				YUVY[(j * width + i) * 2 + 1] = _V[j * ((width + 1) / 2) + i / 2];
			}
		}
	}
	return 0;
}

static int YUV422p_2UYVY(unsigned char* YUV422p, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x59

	int i = 0, j = 0;
	unsigned char* _Y = YUV422p;
	unsigned char* _U = YUV422p + width * height;
	unsigned char* _V = YUV422p + width * height + ((width + 1) / 2) * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
				UYVY[(j * width + i) * 2 + 1] = _U[j * ((width + 1) / 2) + i / 2];
			}else
			{
				UYVY[(j * width + i) * 2 + 1] = _V[j * ((width + 1) / 2) + i / 2];
			}
		}
	}
	return 0;
}

static int YUV422p_2Gray(unsigned char* YUV422p, unsigned char* Gray, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5A

    memcpy_fl(Gray, sizeof(unsigned char) * width * height, YUV422p, sizeof(unsigned char) * width * height);
	return 0;
}

#endif

#if 1
/********************* YUV420sp_NV12  2  ... *********/
static int YUV420sp_NV12_2RGBA(unsigned char* YUV420sp_NV12, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5B

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420sp_NV12_2BGRA(unsigned char* YUV420sp_NV12, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5C

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420sp_NV12_2RGB(unsigned char* YUV420sp_NV12, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5D

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420sp_NV12_2BGR(unsigned char* YUV420sp_NV12, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5E

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV420sp_NV12_2_YUV420p_I420_YU12(unsigned char* YUV420sp_NV12, unsigned char* YUV420p_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x5F

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	unsigned char* __Y = YUV420p_I420_YU12;
	unsigned char* __U = YUV420p_I420_YU12 + width * height;
	unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2];
			__V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV420sp_NV12_2YV12(unsigned char* YUV420sp_NV12, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x60

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	unsigned char* __Y = YV12;
	unsigned char* __V = YV12 + width * height;
	unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2];
			__V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV420sp_NV12_2YUV422p(unsigned char* YUV420sp_NV12, unsigned char* YUV422p, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x61

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	unsigned char* __Y = YUV422p;
	unsigned char* __U = YUV422p + width * height;
	unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 0];
			__V[j * ((width + 1) / 2) + i] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV420sp_NV12_2YUV422sp(unsigned char* YUV420sp_NV12, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x62

	int i = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	unsigned char* __Y = YUV422sp;
	unsigned char* __UV = YUV422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(i = 0; i < height; i ++)
	{
        memcpy_fl(&(__UV[i * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2, &(_UV[(i / 2) * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2);
	}
	return 0;
}

static int YUV420sp_NV12_2_YVU420sp_NV21(unsigned char* YUV420sp_NV12, unsigned char* YVU420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x63

	int i = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	unsigned char* __Y = YVU420sp_NV12;
	unsigned char* __VU = YVU420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(i = 0; i < (((width + 1) / 2) * ((height + 1) / 2)); i ++)
	{
		__VU[i * 2    ] = _UV[i * 2 + 1];
		__VU[i * 2 + 1] = _UV[i * 2    ];
	}
	return 0;
}

static int YUV420sp_NV12_2YVU422sp(unsigned char* YUV420sp_NV12, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x64

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;
	
	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YUV420sp_NV12_2YUVY(unsigned char* YUV420sp_NV12, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x65

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
			}else
			{
				YUVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}
		}
	}
	return 0;
}

static int YUV420sp_NV12_2UYVY(unsigned char* YUV420sp_NV12, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x66

	int i = 0, j = 0;
	unsigned char* _Y = YUV420sp_NV12;
	unsigned char* _UV = YUV420sp_NV12 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
				UYVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2];
			}else
			{
				UYVY[(j * width + i) * 2 + 1] = _UV[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}
		}
	}
	return 0;
}

static int YUV420sp_NV12_2Gray(unsigned char* YUV420sp_NV12, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x67

    memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YUV420sp_NV12, sizeof(unsigned char) * width * height);
	return 0;
}

#endif

#if 1
/************ YUV422sp  2  ... ********/
static int YUV422sp_2RGBA(unsigned char* YUV422sp, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x68

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422sp_2BGRA(unsigned char* YUV422sp, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x69

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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


static int YUV422sp_2RGB(unsigned char* YUV422sp, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6A

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422sp_2BGR(unsigned char* YUV422sp, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6B

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
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

static int YUV422sp_2_YUV420p_I420_YU12(unsigned char* YUV422sp, unsigned char* YUV420p_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6C

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	unsigned char* __Y = YUV420p_I420_YU12;
	unsigned char* __U = YUV420p_I420_YU12 + width * height;
	unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2];
			__V[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV422sp_2YV12(unsigned char* YUV422sp, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6D

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	unsigned char* __Y = YV12;
	unsigned char* __V = YV12 + width * height;
	unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2];
			__V[j * ((width + 1) / 2) + i] = _UV[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV422sp_2YUV422p(unsigned char* YUV422sp, unsigned char* YUV422p, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6E

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	unsigned char* __Y = YUV422p;
	unsigned char* __U = YUV422p + width * height;
	unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 0];
			__V[j * ((width + 1) / 2) + i] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
		}
	}
	return 0;
}

static int YUV422sp_2_YUV420sp_NV12(unsigned char* YUV422sp, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x6F

	int i = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	unsigned char* __Y = YUV420sp_NV12;
	unsigned char* __UV = YUV420sp_NV12 + width * height;

    memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(i = 0; i < (height + 1) / 2; i ++)
	{
        memcpy_fl(&(__UV[i * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2, &(_UV[(i * 2) * ((width + 1) / 2) * 2]), sizeof(unsigned char) * ((width + 1) / 2) * 2);
	}
	return 0;
}

static int YUV422sp_2_YVU420sp_NV21(unsigned char* YUV422sp, unsigned char* YVU420sp_NV21, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x70

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	unsigned char* __Y = YVU420sp_NV21;
	unsigned char* __VU = YVU420sp_NV21 + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < ((width + 1) / 2); i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _UV[((j * 2) * ((width + 1) / 2) + i) * 2 + 1];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[((j * 2) * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}


static int YUV422sp_2YVU422sp(unsigned char* YUV422sp, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x71

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;
	
	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _UV[(j * ((width + 1) / 2) + i) * 2 + 1];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YUV422sp_2YUVY(unsigned char* YUV422sp, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x72

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
			}else
			{
				YUVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}
		}
	}
	return 0;
}

static int YUV422sp_2UYVY(unsigned char* YUV422sp, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x72

	int i = 0, j = 0;
	unsigned char* _Y = YUV422sp;
	unsigned char* _UV = YUV422sp + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
				UYVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2];
			}else
			{
				UYVY[(j * width + i) * 2 + 1] = _UV[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}
		}
	}
	return 0;
}

static int YUV422sp_2Gray(unsigned char* YUV422sp, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x73

	memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YUV422sp, sizeof(unsigned char) * width * height);
	return 0;
}

#endif

#if 1
/************ YVU420sp_NV21  2  ... ********/
static int YVU420sp_NV21_2RGBA(unsigned char* YVU420sp_NV21, unsigned char* RGBA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x74

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			unsigned char Y, U, V;
			float R, G, B;
			Y = _Y[j * width + i];
			U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2    ];
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

static int YVU420sp_NV21_2BGRA(unsigned char* YVU420sp_NV21, unsigned char* BGRA, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x75

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			unsigned char Y, U, V;
			float R, G, B;
			Y = _Y[j * width + i];
			U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2    ];
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

static int YVU420sp_NV21_2RGB(unsigned char* YVU420sp_NV21, unsigned char* RGB, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x76

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			unsigned char Y, U, V;
			float R, G, B;
			Y = _Y[j * width + i];
			U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2    ];
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

static int YVU420sp_NV21_2BGR(unsigned char* YVU420sp_NV21, unsigned char* BGR, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x77

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			unsigned char Y, U, V;
			float R, G, B;
			Y = _Y[j * width + i];
			U = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			V = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2    ];
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

static int YVU420sp_NV21_2_YUV420p_I420_YU12(unsigned char* YVU420sp_NV21, unsigned char* YUV420p_I420_YU12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x78

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	unsigned char* __Y = YUV420p_I420_YU12;
	unsigned char* __U = YUV420p_I420_YU12 + width * height;
	unsigned char* __V = YUV420p_I420_YU12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
			__V[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YVU420sp_NV21_2YV12(unsigned char* YVU420sp_NV21, unsigned char* YV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x79

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	unsigned char* __Y = YV12;
	unsigned char* __V = YV12 + width * height;
	unsigned char* __U = YV12 + width * height + ((width + 1) / 2) * ((height + 1) / 2);

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);

	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2 + 1];
			__V[j * ((width + 1) / 2) + i] = _VU[(j * 2 * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YVU420sp_NV21_2YUV422p(unsigned char* YVU420sp_NV21, unsigned char* YUV422p, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7A

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	unsigned char* __Y = YUV422p;
	unsigned char* __U = YUV422p + width * height;
	unsigned char* __V = YUV422p + width * height + ((width + 1) / 2) * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__U[j * ((width + 1) / 2) + i] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
			__V[j * ((width + 1) / 2) + i] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 0];
		}
	}
	return 0;
}


static int YVU420sp_NV21_2_YUV420sp_NV12(unsigned char* YVU420sp_NV21, unsigned char* YUV420sp_NV12, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7B

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	unsigned char* __Y = YUV420sp_NV12;
	unsigned char* __UV = YUV420sp_NV12 + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < (height + 1) / 2; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__UV[(j * ((width + 1) / 2) + i) * 2    ] = _VU[(j * ((width + 1) / 2) + i) * 2 + 1];
			__UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}


static int YVU420sp_NV21_2_YUV422sp(unsigned char* YVU420sp_NV21, unsigned char* YUV422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7C

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	unsigned char* __Y = YUV422sp;
	unsigned char* __UV = YUV422sp + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < ((width + 1) / 2); i ++)
		{
			__UV[(j * ((width + 1) / 2) + i) * 2    ] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
			__UV[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YVU420sp_NV21_2YVU422sp(unsigned char* YVU420sp_NV21, unsigned char* YVU422sp, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7D

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;
	
	unsigned char* __Y = YVU422sp;
	unsigned char* __VU = YVU422sp + width * height;

	memcpy_fl(__Y, sizeof(unsigned char) * width * height, _Y, sizeof(unsigned char) * width * height);
	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < (width + 1) / 2; i ++)
		{
			__VU[(j * ((width + 1) / 2) + i) * 2    ] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2 + 1];
			__VU[(j * ((width + 1) / 2) + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + i) * 2    ];
		}
	}
	return 0;
}

static int YVU420sp_NV21_2YUVY(unsigned char* YVU420sp_NV21, unsigned char* YUVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7E

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			YUVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 0)
			{
				YUVY[(j * width + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}else
			{
				YUVY[(j * width + i) * 2 + 1] = _VU[((j / 2) * ((width + 1) / 2) + (i / 2)) * 2 + 0];
			}
		}
	}
	return 0;
}

static int YVU420sp_NV21_2UYVY(unsigned char* YVU420sp_NV21, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x7F

	int i = 0, j = 0;
	unsigned char* _Y = YVU420sp_NV21;
	unsigned char* _VU = YVU420sp_NV21 + width * height;

	for(j = 0; j < height; j ++)
	{
		for(i = 0; i < width; i ++)
		{
			UYVY[(j * width + i) * 2] = _Y[j * width + i];
			if(i % 2 == 1)
			{
				UYVY[(j * width + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 1];
			}else
			{
				UYVY[(j * width + i) * 2 + 1] = _VU[(j * ((width + 1) / 2) + (i / 2)) * 2 + 0];
			}
		}
	}
	return 0;
}

static int YVU420sp_NV21_2Gray(unsigned char* YVU420sp_NV21, unsigned char* UYVY, int width, int height){

#undef FUNC_CODE
#define FUNC_CODE 0x80

	memcpy_fl(UYVY, sizeof(unsigned char) * width * height, YVU420sp_NV21, sizeof(unsigned char) * width * height);
	return 0;
}

#endif

static int RGBA2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x81

	switch(oFmt)
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

static int BGRA2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x82

	switch(oFmt)
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

static int RGB2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x83

	switch(oFmt)
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

static int BGR2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x84

	switch(oFmt)
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

static int YUV420P_I420_YU12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x85

	switch(oFmt)
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

static int YV12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x86

	switch(oFmt)
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




static int YUV422p_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x87

	switch(oFmt)
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

static int YUV420sp_NV12_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x88

	switch(oFmt)
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

static int YUV422sp_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x89

	switch(oFmt)
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

static int YVU420sp_NV21_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt){
#undef FUNC_CODE
#define FUNC_CODE 0x8A

	switch(oFmt)
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



static int YVYU_2YUV420p(unsigned char*YVYU, unsigned char*YUV420p, int width, int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2, *V2;

    unsigned char * p = NULL;

    //     int src_size = (cap->GetPreWidth()*cap->GetPreHeight()) << 1;      //源图片,格式为YUY2，大小src_width * src_height *2
    //     unsigned char* src_buf = (unsigned char *)malloc(src_size * sizeof(char));
    //     memcpy(src_buf, buf, src_size);

    p = YVYU;
    int tem_size = (width*height) << 1;    //此处开辟一块与源图大小相同的一块区域。本文中首先将packed形式的YUY2格式数据，转变

                                           //为planar形式的术据，并将这些数据存放在此处开辟的内存中。总体而言，此处区域起临时存储作用。

                                           //为后序构建YUV420数据服务。
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);

    //     int out_size = cap->GetPreWidth()*cap->GetPreHeight() * 1.5;        //开辟src_width*src_height * 1.5大小区域，用于保存YUV420数据。
    //     unsigned char* out_buf = (unsigned char *)malloc(out_size * sizeof(char));
    //     memset(out_buf, 0, out_size);

    //将临时存储区域中Y、U、V各个分量的首地址确定。
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);   //Y  U  V  =4 : 2 ; 2  


                                   //将YUV420存储区域中Y、U、V各个分量的首地址确定。
    Y2 = YUV420p;
    U2 = Y2 + width*height;      //长与宽反映象素的各数，每个像素中都一定包含Y分量，所以要预留长*宽的空间给Y分量存储。
    V2 = U2 + (width*height >> 2);

    /*由打包YUYV变成平板YUV*/
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (height >> 1); ++j)
        {
            Y[j * 2] = p[4 * j];
            U[j] = p[4 * j + 3];
            Y[j * 2 + 1] = p[4 * j + 2];
            V[j] = p[4 * j + 1];
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
        memcpy(U2, U, height >> 1);
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
    return 0;
}



static int YUYV_2YUV420p(unsigned char*YUYV, unsigned char*YUV420p,int width,int height)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    unsigned char *Y, *U, *V;
    unsigned char * Y2, *U2, *V2;

    unsigned char * p = NULL;

    //     int src_size = (cap->GetPreWidth()*cap->GetPreHeight()) << 1;      //源图片,格式为YUY2，大小src_width * src_height *2
    //     unsigned char* src_buf = (unsigned char *)malloc(src_size * sizeof(char));
    //     memcpy(src_buf, buf, src_size);

    p = YUYV;
    int tem_size = (width*height) << 1;    //此处开辟一块与源图大小相同的一块区域。本文中首先将packed形式的YUY2格式数据，转变

                                           //为planar形式的术据，并将这些数据存放在此处开辟的内存中。总体而言，此处区域起临时存储作用。

                                           //为后序构建YUV420数据服务。
    unsigned char* tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);

    //     int out_size = cap->GetPreWidth()*cap->GetPreHeight() * 1.5;        //开辟src_width*src_height * 1.5大小区域，用于保存YUV420数据。
    //     unsigned char* out_buf = (unsigned char *)malloc(out_size * sizeof(char));
    //     memset(out_buf, 0, out_size);

    //将临时存储区域中Y、U、V各个分量的首地址确定。
    Y = tem_buf;
    U = Y + width*height;
    V = U + (width*height >> 1);   //Y  U  V  =4 : 2 ; 2  


                                                             //将YUV420存储区域中Y、U、V各个分量的首地址确定。
    Y2 = YUV420p;
    U2 = Y2 + width*height;      //长与宽反映象素的各数，每个像素中都一定包含Y分量，所以要预留长*宽的空间给Y分量存储。
    V2 = U2 + (width*height >> 2);

    /*由打包YUYV变成平板YUV*/
    int k, j;
    for (k = 0; k < height; ++k)
    {
        for (j = 0; j < (height >> 1); ++j)
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
        memcpy(U2, U,height >> 1);
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
    return 0;
}


// static int YUV422sp_2YUV422p(unsigned char*yuv422sp, unsigned char*yuv422p, int width, int height)
// {
//     int i, j;
//     int y_size;
//     int uv_size;
//     unsigned char* p_y1;
//     unsigned char* p_uv;
// 
//     unsigned char* p_y2;
//     unsigned char* p_u;
//     unsigned char* p_v;
// 
//     y_size = uv_size = width*height;
//     p_y1 = yuv422sp;
//     p_uv = yuv422sp + y_size;
// 
//     p_y2 = yuv422p;
//     p_u = yuv422p + y_size;
//     p_v = p_u + width*height / 2;
// 
//     memcpy(p_y2, p_y1, y_size);
// 
//     for (j = 0, i = 0; j < uv_size; j += 2, i++)
//     {
//         p_u[i] = p_uv[j];
//         p_v[i] = p_uv[j + 1];
//     }
// }






static int YUYV_2RGBA(unsigned char*YUYV, unsigned char*RGBA, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YUYV_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_RGBA(YUV420p, RGBA, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YUYV_2BGRA(unsigned char*YUYV, unsigned char*BGRA, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YUYV_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为BGRA
    YUV420P_I420_YU12_2_BGRA(YUV420p,BGRA, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YUYV_2RGB(unsigned char*YUYV, unsigned char*RGB, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YUYV_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_RGB(YUV420p, RGB, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}

static int YUYV_2BGR(unsigned char*YUYV, unsigned char* BGR, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YUYV_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_BGR(YUV420p,BGR, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YUYV_2YV12(unsigned char*YUYV, unsigned char* YV12, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YUYV_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为YV12
    YUV420P_I420_YU12_2_YV12(YUV420p,YV12, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YUVY_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUYV_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YUYV_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUYV_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YUYV_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YUYV_2YV12(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;
}




static int YVYU_2RGBA(unsigned char*YUYV, unsigned char*RGBA, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YVYU_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_RGBA(YUV420p, RGBA, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YVYU_2BGRA(unsigned char*YUYV, unsigned char*BGRA, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YVYU_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为BGRA
    YUV420P_I420_YU12_2_BGRA(YUV420p, BGRA, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YVYU_2RGB(unsigned char*YUYV, unsigned char*RGB, int width, int height)
{
    //先将YUYV转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YVYU_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_RGB(YUV420p, RGB, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}


static int YVYU_2BGR(unsigned char*YUYV, unsigned char* BGR, int width, int height)
{
    //先将UYVY转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YVYU_2YUV420p(YUYV, YUV420p, width, height);


    //再将YUV420p转换为RGBA
    YUV420P_I420_YU12_2_BGR(YUV420p, BGR, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}




static int YVYU_2YV12(unsigned char*YVYU, unsigned char* YV12, int width, int height)
{
    //先将UYVY转化为YUV420p
    unsigned char*YUV420p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*1.5);


    YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为YV12
    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);
    if (YUV420p)
    {
        free(YUV420p);
        YUV420p = NULL;
    }
    return 0;
}





static int UYVY_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVYU_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVYU_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVYU_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVYU_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YVYU_2YV12(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;
}



static int YVU_422sp_2YV12(unsigned char*YVU_422sp, unsigned char* YV12, int width, int height)
{
    //先将YVU_422sp转化为YUV422p
    unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height*2);

    YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
 //   YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为YV12
   
    YUV422p_2YV12(YUV422p, YV12, width, height);

//    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);

    if (YUV422p)
    {
        free(YUV422p);
        YUV422p = NULL;
    }
    return 0;
}


static int YVU_422sp_2RGB(unsigned char*YVU_422sp, unsigned char* RGB, int width, int height)
{
    //先将YVU_422sp转化为YUV422p
    unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);

    YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
    //   YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为YV12

    YUV422p_2RGB(YUV422p, RGB, width, height);

    //    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);

    if (YUV422p)
    {
        free(YUV422p);
        YUV422p = NULL;
    }
    return 0;
}


static int YVU_422sp_2BGR(unsigned char*YVU_422sp, unsigned char* BGR, int width, int height)
{
    //先将YVU_422sp转化为YUV422p
    unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);

    YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
    //   YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为YV12

    YUV422p_2BGR(YUV422p,BGR, width, height);

    //    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);

    if (YUV422p)
    {
        free(YUV422p);
        YUV422p = NULL;
    }
    return 0;
}



static int YVU_422sp_2BGRA(unsigned char*YVU_422sp, unsigned char* BGRA, int width, int height)
{
    //先将YVU_422sp转化为YUV422p
    unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);

    YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
    //   YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为YV12

    YUV422p_2BGRA(YUV422p, BGRA, width, height);

    //    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);

    if (YUV422p)
    {
        free(YUV422p);
        YUV422p = NULL;
    }
    return 0;
}

static int YVU_422sp_2RGBA(unsigned char*YVU_422sp, unsigned char* RGBA, int width, int height)
{
    //先将YVU_422sp转化为YUV422p
    unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);

    YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
    //   YVYU_2YUV420p(YVYU, YUV420p, width, height);


    //再将YUV420p转换为RGBA

    YUV422p_2RGBA(YUV422p, RGBA, width, height);

    //    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);

    if (YUV422p)
    {
        free(YUV422p);
        YUV422p = NULL;
    }
    return 0;
}


// static int YUV422sp_2YUVY(unsigned char*YVU_422sp, unsigned char* YUVY, int width, int height)
// {
//     //先将YVU_422sp转化为YUV422p
//     unsigned char*YUV422p = (unsigned char*)malloc(sizeof(unsigned char)*width*height * 2);
// 
//     YUV422sp_2YUV422p(YVU_422sp, YUV422p, width, height);
//     //   YVYU_2YUV420p(YVYU, YUV420p, width, height);
// 
//     //再将YUV420p转换为YUVY
// 
//     YUV422p_2YUVY(YUV422p, YUVY, width, height);
// 
//     //    YUV420P_I420_YU12_2_YV12(YUV420p, YV12, width, height);
// 
//     if (YUV422p)
//     {
//         free(YUV422p);
//         YUV422p = NULL;
//     }
//     return 0;
// }

static int YVU_422sp_2(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt oFmt)
{
#undef FUNC_CODE
#define FUNC_CODE 0x8B
    switch (oFmt)
    {
    case Alva_FMT_RGBA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVU_422sp_2RGBA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGRA_8888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 4);
        YVU_422sp_2BGRA(iImage, *oImage, width, height);
        break;
    case Alva_FMT_RGB_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVU_422sp_2RGB(iImage, *oImage, width, height);
        break;
    case Alva_FMT_BGR_888:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
        YVU_422sp_2BGR(iImage, *oImage, width, height);
        break;
    case Alva_FMT_YV12:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + ((width + 1) / 2) * ((height + 1) / 2) * 2));
        YVU_422sp_2YV12(iImage, *oImage, width, height);
        break;
    default:
        *oImage = (unsigned char*)calloc(1, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        memcpy_fl(*oImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2), iImage, sizeof(unsigned char) * (width * height + (((width + 1) / 2) * height) * 2));
        break;
    }
    return 0;

}

int ImageTypeConverter(unsigned char* iImage, unsigned char** oImage, int width, int height, alvafmt iFmt, alvafmt oFmt){

#undef FUNC_CODE
#define FUNC_CODE 0x8B

	switch (iFmt)
	{
        case Alva_FMT_YUVY:
            YUVY_2(iImage,oImage,width, height,oFmt);
            break;
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

			break;
		case Alva_FMT_UYVY:
            UYVY_2(iImage, oImage, width, height, oFmt);
			break;
		default://Alva_FMT_Gray
			break;
	}
	return 0;
}




