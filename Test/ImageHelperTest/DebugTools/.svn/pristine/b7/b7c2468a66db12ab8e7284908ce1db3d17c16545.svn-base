#include "Config.h"

#include "stdio_fl.h"
#include "string_fl.h"

#include <stdarg.h>
#if OS_ALVA==OS_Android || OS_ALVA==OS_Linux
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#elif OS_ALVA == OS_iOS
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#elif OS_ALVA == OS_Windows
#include <io.h>
#include <direct.h>
#endif

#include "Format.h"

#define LOG_TAG "DUMP"
#include "Log.h"
#include "ErrorTools.h"

#include "AndroidHelper.h"

#include "BmpHelper.h"

#include "Dump.h"

#undef FILE_NUM
#define FILE_NUM 0x8200

#if OS_ALVA == OS_Android
static char* dumpRoot = "/sdcard/Alva/";
#elif OS_ALVA == OS_iOS
static char* dumpRoot = "./";
#elif OS_ALVA == OS_Windows
static char* dumpRoot = ".\\";
#elif OS_ALVA == OS_Linux
static char* dumpRoot = "/var/AlvaDump/";
#endif

char* dumpbuffer = NULL;

int dump_init_temp(size_t buffersize){

#undef FUNC_CODE
#define FUNC_CODE 0x01

    //create dump folder
#if OS_ALVA==OS_Windows
    if(_access(dumpRoot, 00) != 0){
        _mkdir(dumpRoot);
#else
    if(access(dumpRoot, 00) != 0){
        mkdir(dumpRoot, S_IRUSR | S_IWUSR);
#endif
    }

    if(NULL != dumpbuffer)
        free(dumpbuffer);
    dumpbuffer = (char*)malloc(buffersize);

    return 0;
}

void* start_filedump_temp(char* filename, DUMP_MODE mode) {

#undef FUNC_CODE
#define FUNC_CODE 0x02

    FILE* file;
    char file2open[1024];

#if OS_ALVA == OS_Android || OS_ALVA == OS_Linux || OS_ALVA == OS_iOS || OS_ALVA == OS_MacOS
    if (filename[0] == '/' || (filename[0] == '\\' && filename[1] == '\\'))
#elif OS_ALVA == OS_Windows
    if (filename[1] == ':' || (filename[0] == '\\' && filename[1] == '\\'))
#endif
        sprintf_fl(file2open, 1024, "%s", filename);
    else
        sprintf_fl(file2open, 1024, "%s%s", dumpRoot, filename);

    if(mode == DUMP_APPEND)
        fopen_fl(&file, file2open, "a+");
    else
        fopen_fl(&file, file2open, "wb");

    if (file == NULL) {
        LOGE("can't open file: %s\n", file2open);
        return NULL;
    }

    if (mode == DUMP_APPEND)
        fseek(file, 0, SEEK_END);

    return file;
}

void dumpbuffer2file_temp(void* file, void* buf, size_t size) {

#undef FUNC_CODE
#define FUNC_CODE 0x03

    if (NULL != file)
        fwrite(buf, 1, size, (FILE*)file);
}

void log2file_temp(void* file, char* format, ...) {

#undef FUNC_CODE
#define FUNC_CODE 0x04

    if (NULL != file) {
        va_list _ArgList;
        va_start(_ArgList, format);
        vfprintf_fl(file, format, _ArgList);
        va_end(_ArgList);
    }
}

int end_filedump_temp(void* file) {

#undef FUNC_CODE
#define FUNC_CODE 0x05

    FILE* tfile = (FILE*)file;
    if (NULL != tfile)
        fclose(tfile);
    return 0;
}

int dump_temp(char* filename, void* buf, size_t size){

#undef FUNC_CODE
#define FUNC_CODE 0x06

    void* file = start_filedump_temp(filename, DUMP_CLEAR);
    if (NULL == file)
        OWN_ERROR_RETURN(0x001, "start_filedump_temp");

    dumpbuffer2file_temp(file, buf, size);

    end_filedump_temp(file);

    return 0;
}

static int dump_bmp_float(char* filename, void* buf, int width, int height, int channels){

#undef FUNC_CODE
#define FUNC_CODE 0x07

	BMP mImage;
	int i, j, z;
	float* floatData = (float*)buf;
	float maxValue = -100000.0f, minValue = 100000.0f;

	mImage.channels = 3;
	mImage.width = width;
	mImage.height = height;
	mImage.data = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
	//求出最大值与最小值
	if(channels != 4){
		for(i = 0; i < height; i ++){
			for(j = 0; j < width; j ++){
				for(z = 0; z < channels; z ++){
					float t = floatData[(i * width + j) * channels + z];
					if(t > maxValue){
						maxValue = t;
					}
					if(t < minValue){
						minValue = t;
					}
				}
			}
		}
	}
    else{
		for(i = 0; i < height; i ++){
			for(j = 0; j < width; j ++){
				for(z = 0; z < 3; z ++){
					float t = floatData[(i * width + j) * 4 + z];
					if(t > maxValue){
						maxValue = t;
					}
					if(t < minValue){
						minValue = t;
					}
				}
			}
		}
	}
	switch (channels){
	    case 1:
			for(i = 0; i < height * width * 3; i += 3){
				unsigned char t = (unsigned char)((int)((floatData[i / 3] - minValue) / (maxValue - minValue) * 255.0f));
				mImage.data[i] = t;
				mImage.data[i + 1] = t;
				mImage.data[i + 2] = t;
			}
		    break;
	    case 3:
			for(i = 0; i < height * width * 3; i += 3){
				unsigned char tR = (unsigned char)((int)((floatData[i + 0] - minValue) / (maxValue - minValue) * 255.0f));
				unsigned char tG = (unsigned char)((int)((floatData[i + 1] - minValue) / (maxValue - minValue) * 255.0f));
				unsigned char tB = (unsigned char)((int)((floatData[i + 2] - minValue) / (maxValue - minValue) * 255.0f));
				mImage.data[i] = tR;
				mImage.data[i + 1] = tG;
				mImage.data[i + 2] = tB;
			}
		    break;
	    case 4:
			for(i = 0, j = 0; i < height * width * 3; i += 3, j += 4){
				unsigned char tR = (unsigned char)((int)((floatData[j + 0] - minValue) / (maxValue - minValue) * 255.0f));
				unsigned char tG = (unsigned char)((int)((floatData[j + 1] - minValue) / (maxValue - minValue) * 255.0f));
				unsigned char tB = (unsigned char)((int)((floatData[j + 2] - minValue) / (maxValue - minValue) * 255.0f));
				mImage.data[i] = tR;
				mImage.data[i + 1] = tG;
				mImage.data[i + 2] = tB;
			}
		    break;
	    default:
		    break;
	}

    i = WriteBMP(filename, &mImage, BIT24);
	free(mImage.data);
    ERROR_RETURN(i, "WriteBMP");

	return 0;
}

static int dump_bmp_uChar(char* filename, void* buf, int width, int height, int channels){

#undef FUNC_CODE
#define FUNC_CODE 0x08

	BMP mImage;
	int i, j;
	unsigned char* UcharData = (unsigned char*)buf;

	mImage.channels = 3;
	mImage.width = width;
	mImage.height = height;
	mImage.data = (unsigned char*)calloc(1, sizeof(unsigned char) * width * height * 3);
	//求出最大值与最小值
	
	switch (channels){
        case 1:
		    for(i = 0; i < height * width * 3; i += 3){
			    unsigned char t = UcharData[i / 3];
			    mImage.data[i] = t;
			    mImage.data[i + 1] = t;
			    mImage.data[i + 2] = t;
		    }
	        break;
        case 3:
		    for(i = 0; i < height * width * 3; i += 3){
			    unsigned char tR = UcharData[i + 0];
			    unsigned char tG = UcharData[i + 1];
			    unsigned char tB = UcharData[i + 2];
			    mImage.data[i] = tR;
			    mImage.data[i + 1] = tG;
			    mImage.data[i + 2] = tB;
		    }
	        break;
        case 4:
		    for(i = 0, j = 0; i < height * width * 3; i += 3, j += 4){
			    unsigned char tR = UcharData[j + 0];
			    unsigned char tG = UcharData[j + 1];
			    unsigned char tB = UcharData[j + 2];
			    mImage.data[i] = tR;
			    mImage.data[i + 1] = tG;
			    mImage.data[i + 2] = tB;
		    }
	        break;
        default:
	        break;
	}

    i = WriteBMP(filename, &mImage, BIT24);
	free(mImage.data);
    ERROR_RETURN(i, "WriteBMP");

	return 0;
}

int dump_bmp(char* filename, void* buf, int width, int height, int channels, DUMPDATATYPE DATATYPE){

#undef FUNC_CODE
#define FUNC_CODE 0x09

    char file2open[1024];

#if OS_ALVA == OS_Android || OS_ALVA == OS_Linux || OS_ALVA == OS_iOS || OS_ALVA == OS_MacOS
    if (filename[0] == '/' || (filename[0] == '\\' && filename[1] == '\\'))
#elif OS_ALVA == OS_Windows
    if (filename[1] == ':' || (filename[0] == '\\' && filename[1] == '\\'))
#endif
        sprintf_fl(file2open, 1024, "%s", filename);
    else
        sprintf_fl(file2open, 1024, "%s%s", dumpRoot, filename);

	switch(DATATYPE){
	    case DUMP_FLOAT:
		    dump_bmp_float(file2open, buf, width, height, channels);
		    break;
	    case DUMP_UCHAR:
		    dump_bmp_uChar(file2open, buf, width, height, channels);
		    break;
	    default:
            OWN_ERROR_RETURN(0x001, "unknown DataType");
		    break;
	}
	return 0;
}

static int dump_matrix_float(char* filename, void* buf, int col, int row) {
#undef FUNC_CODE
#define FUNC_CODE 0x0A

    float* matrix = (float*)buf;

    int r, c;

    void* file = start_filedump_temp(filename, DUMP_CLEAR);
    if (NULL == file)
        OWN_ERROR_RETURN(0x001, "start_filedump_temp");

    for (r = 0; r < row; r++) {
        for (c = 0; c < col; c++) {
            log2file_temp(file, "%f, ", matrix[r * col + c]);
        }
        log2file_temp(file, "\n");
    }

    end_filedump_temp(file);

    return 0;
}

int dump_matrix(char* filename, void* buf, int col, int row, DUMPDATATYPE DATATYPE) {
#undef FUNC_CODE
#define FUNC_CODE 0x0B

    int err = 0;

    switch (DATATYPE) {
        case DUMP_FLOAT:
            err = dump_matrix_float(filename, buf, col, row);
            break;
        default:
            break;
    }

    return err;
}

static int load_matrix_float(char* filename, void* buf, int col, int row) {
#undef FUNC_CODE
#define FUNC_CODE 0x0C
    float* matrix = (float*)buf;
    char* filedata = NULL, *ptr = NULL, *lastPtr = NULL;
    int filelength = 0;

    int r = 0, c = 0;

    int tRow = row;

    int err = readData(filename, Alva_File, &filedata, &filelength);
    ERROR_RETURN(err, "readData");

    if (tRow == -1) {
        tRow = 0;
        lastPtr = filedata;
        while (1) {
            ptr = strchr(lastPtr, '\n');
            if (ptr) {
                tRow++;
            }
            else {
                if (*(lastPtr) >= '0' && *(lastPtr) <= '9') {
                    tRow++;
                }
                break;
            }
            ptr++;
            lastPtr = ptr;
        }
    }

    ptr = filedata;
    for (r = 0; r < tRow; r++) {
        for (c = 0; c < col; c++) {
            sscanf_fl(ptr, "%f", &(matrix[r * col + c]));
            ptr = strchr(ptr, ',') + 1;
        }
    }

    free(filedata);

    return tRow;
}

int load_matrix(char* filename, void* buf, int col, int row, DUMPDATATYPE DATATYPE) {
#undef FUNC_CODE
#define FUNC_CODE 0x0D

    int err = -1;

    switch (DATATYPE) {
        case DUMP_FLOAT:
            err = load_matrix_float(filename, buf, col, row);
            break;
        default:
            break;
    }

    return err;
}
