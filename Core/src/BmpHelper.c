#include "Config.h"

#include <stdlib.h>
#include "stdio_fl.h"
#include "memory_fl.h"

#define LOG_TAG "BmpHelper"
#include "Log.h"
#include "ErrorTools.h"

#include "BmpHelper.h"

#undef FILE_NUM
#define FILE_NUM 0xF100

typedef struct _BMPFILEHEAD_ {
    unsigned char   type[2];        //存储 'B' 'M'                    2字节
    unsigned int    size;           //位图文件大小                    4字节
    unsigned short  reserved1;      //保留字                          2字节
    unsigned short  reserved2;      //保留字                          2字节
    unsigned int    offBits;        //位图数据起始位置                4字节
}BMPHEAD;
typedef struct _BMPFILEINFOHEAD_ {
    unsigned int    selfSize;       //位图信息头的大小                 4字节
    long            bitWidth;       //位图的宽度,以像素为单位          4字节
    long            bitHeight;      //位图的高度,以像素为单位          4字节
    unsigned short  bitPlanes;      //目标设备的级别,必须为1           2字节
    unsigned short  pixelBitCount;  //每个像素所需的位数               2字节
    unsigned int    compression;    //位图压缩类型,0(不压缩)           4字节
    unsigned int    sizeImage;      //位图的大小,以字节为单位          4字节
    long            pixelXPerMeter; //位图的水平分辨率,每米像素数      4字节
    long            pixelYPerMeter; //位图的垂直分辨率,每米像素数      4字节
    unsigned int    colorUsed;      //位图实际使用的颜色表中的颜色数   4字节
    unsigned int    colorImportant; //位图显示过程中重要的颜色数       4字节
}BMPINFOHEAD;

#define BMP_SIZE_FILEHEADER 14
#define BMP_SIZE_INFOHEADER 40

#define BMP_COLOR_BITS_24 24
#define BMP_COLOR_BITS_32 32

static unsigned int uInt16Number(unsigned char buf[2]){

#undef FUNC_CODE
#define FUNC_CODE 0x01

    return (buf[1] << 8) | buf[0];
}

static unsigned int uInt32Number(unsigned char buf[4]){

#undef FUNC_CODE
#define FUNC_CODE 0x02

    unsigned numb = buf[3];
    numb = (numb << 8) | buf[2];
    numb = (numb << 8) | buf[1];
    numb = (numb << 8) | buf[0];
    return numb;
}

static void IntTo2Bytes(int val, unsigned char buf[2]){

#undef FUNC_CODE
#define FUNC_CODE 0x03

    buf[0] = (unsigned char)val;
    buf[1] = (unsigned char)(val >> 8);
}

static void IntTo4Bytes(int val, unsigned char buf[4]){

#undef FUNC_CODE
#define FUNC_CODE 0x04

    buf[0] = (unsigned char)val;
    buf[1] = (unsigned char)(val >> 8);
    buf[2] = (unsigned char)(val >> 16);
    buf[3] = (unsigned char)(val >> 24);
}

static int ReadFileHeader(FILE* f, int* bitmap_pos){

#undef FUNC_CODE
#define FUNC_CODE 0x05

    unsigned char header[BMP_SIZE_FILEHEADER];

    size_t numb = 0;
    int offset = 0;

    if (fseek(f, 0, SEEK_SET))
        OWN_ERROR_RETURN(0x001, "fseek");

    //读取bmp head信息
    numb = fread(header, BMP_SIZE_FILEHEADER, 1, f);
    if (numb != 1)
        OWN_ERROR_RETURN(0x002, "fread");

    // 0 - 1
    if (header[0] != 'B' || header[1] != 'M')
        OWN_ERROR_RETURN(0x003, "header");

    // 6 - 7
    // 8 - 9

    // 10 - 13
    offset = uInt32Number(header + 10);
    if(offset != 54)
        OWN_ERROR_RETURN(0x004, "offset");

    *bitmap_pos  = offset;
    return 0;
}

static int ReadFileInfoHead(FILE* f, BMP* img){

#undef FUNC_CODE
#define FUNC_CODE 0x06

    unsigned char header[BMP_SIZE_INFOHEADER];
    BMPINFOHEAD bmp_info_head;
    
    size_t numb = 0;

    if (fseek(f, BMP_SIZE_FILEHEADER, SEEK_SET))
        OWN_ERROR_RETURN(0x001, "fseek");

    //读取bmp info head信息
    numb = fread(header, BMP_SIZE_INFOHEADER, 1, f);
    if (numb != 1)
        OWN_ERROR_RETURN(0x002, "fread");
    
    // 14 - 17
    bmp_info_head.selfSize = uInt32Number(header);
    if(bmp_info_head.selfSize != 40)
        OWN_ERROR_RETURN(0x003, "selfSize");

    // 18 - 21
    bmp_info_head.bitWidth = (long)uInt32Number(header + 4);
    // 22 - 25
    bmp_info_head.bitHeight = (long)uInt32Number(header + 8);
    // 26 - 27
    bmp_info_head.bitPlanes = (unsigned short)uInt16Number(header + 12);
    // 28 - 29
    bmp_info_head.pixelBitCount = (unsigned short)uInt16Number(header + 14);
    // 30 - 33
    bmp_info_head.compression = uInt32Number(header + 16);
    // 34 - 37
    bmp_info_head.sizeImage = uInt32Number(header + 20);
    // 38 - 41
    bmp_info_head.pixelXPerMeter = (long)uInt32Number(header + 24);
    // 42 - 45
    bmp_info_head.pixelYPerMeter = (long)uInt32Number(header + 28);
    // 46 - 49
    bmp_info_head.colorUsed = uInt32Number(header + 32);
    // 50 - 53
    bmp_info_head.colorImportant = uInt32Number(header + 36);

    img->width = (int)(bmp_info_head.bitWidth);
    img->height = (int)(bmp_info_head.bitHeight);

    return (int)(bmp_info_head.pixelBitCount);
}

int LoadBMP(const char* file, BMP* out_img, BITDATATYPE bit_data_type){

#undef FUNC_CODE
#define FUNC_CODE 0x07

    FILE* f;
    int bitmap_pos;
    int n_bits;

    int flErr = 0;

    fopen_fl(&f, file, "rb");

    if (!f) {
        LOGE("Can not open: %s\n", file);
        OWN_ERROR_RETURN(0x001, "fopen_fl");
    }

    if (out_img == NULL)
        OWN_ERROR_RETURN(0x002, "out_img");

    if (0 > (flErr = ReadFileHeader(f, &bitmap_pos))){
        fclose(f);
        ERROR_RETURN(flErr, "ReadFileHeader");
    }

    n_bits = ReadFileInfoHead(f, out_img);

    if(n_bits == BMP_COLOR_BITS_24){
        int rgb_size = 0;
        unsigned char* rgb = NULL;
        int y = 0;
        unsigned char* line = NULL;
        int rest_4 = 0;

        if (fseek(f, bitmap_pos, SEEK_SET)){
            fclose(f);
            OWN_ERROR_RETURN(0x003, "fseek");
        }

        rgb_size = 3 * out_img->width;

        rest_4 = rgb_size % 4;
        if (rest_4 > 0)
            rgb_size += 4 - rest_4;

        if(bit_data_type == 1)
        {
            out_img->channels = 4;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 4);

            if (out_img->data == NULL)
                OWN_ERROR_RETURN(0x004, "malloc");

            rgb = (unsigned char*)malloc(rgb_size);

            if(NULL == rgb)
                OWN_ERROR_RETURN(0x005, "malloc");

            for(y = out_img->height - 1; y >= 0; y --){
                size_t numb = 0;
                int x = 0;

                numb = fread(rgb, rgb_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgb);
                    OWN_ERROR_RETURN(0x006, "fread");
                }

                numb = 0;
                line = out_img->data + out_img->width * 4 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[3] = 255;
                    line[2] = rgb[numb++];
                    line[1] = rgb[numb++];
                    line[0]= rgb[numb++];
                    line += 4;
                }
            }
        }else if(bit_data_type == 2)
        {
            out_img->channels = 3;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 3);

            if (out_img->data == NULL)
                OWN_ERROR_RETURN(0x007, "malloc");

            rgb = (unsigned char*)malloc(rgb_size);

            if(NULL == rgb)
                OWN_ERROR_RETURN(0x008, "malloc");

            for(y = out_img->height - 1; y >= 0; y --){
                size_t numb = 0;
                int x = 0;

                numb = fread(rgb, rgb_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgb);
                    OWN_ERROR_RETURN(0x009, "fread");
                }

                numb = 0;
                line = out_img->data + out_img->width * 3 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgb[numb++];
                    line[1] = rgb[numb++];
                    line[0]= rgb[numb++];
                    line += 3;
                }
            }
        }
        fclose(f);
        free(rgb);
    }else if(n_bits == BMP_COLOR_BITS_32)
    {
        int rgba_size = 0;
        unsigned char* rgba = NULL;
        int y = 0;
        unsigned char* line = NULL;

        if (fseek(f, bitmap_pos, SEEK_SET)){
            fclose(f);
            OWN_ERROR_RETURN(0x00A, "fseek");
        }

        rgba_size = 4 * out_img->width;
        if(bit_data_type == 1)
        {
            out_img->channels = 4;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 4);

            if (out_img->data == NULL)
                OWN_ERROR_RETURN(0x00B, "malloc");

            rgba = (unsigned char*)malloc(rgba_size);

            if(NULL == rgba)
                OWN_ERROR_RETURN(0x00C, "malloc");

            for (y = out_img->height - 1; y >= 0; y --){
                size_t numb = 0;
                int x = 0;

                numb = fread(rgba, rgba_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgba);
                    OWN_ERROR_RETURN(0x00D, "fread");
                }

                numb = 0;
                line = out_img->data + out_img->width * 4 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgba[numb++];//B
                    line[1] = rgba[numb++];//G
                    line[0] = rgba[numb++];//R
                    line[3]= rgba[numb++];//A
                    line += 4;
                }
            }
        }else if(bit_data_type == 2)
        {
            out_img->channels = 3;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 3);

            if (out_img->data == NULL)
                OWN_ERROR_RETURN(0x00E, "malloc");

            rgba = (unsigned char*)malloc(rgba_size);

            if(NULL == rgba)
                OWN_ERROR_RETURN(0x00F, "malloc");

            for (y = out_img->height - 1; y >= 0; y --){
                size_t numb = 0;
                int x = 0;

                numb = fread(rgba, rgba_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgba);
                    OWN_ERROR_RETURN(0x010, "fread");
                }

                numb = 0;
                line = out_img->data + out_img->width * 3 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgba[numb++];//B
                    line[1] = rgba[numb++];//G
                    line[0] = rgba[numb++];//R
                    line += 3;
                    numb ++;
                }
            }
        }
        fclose(f);
        free(rgba);
    }
    else{
        OWN_ERROR_RETURN(0x011, "unknown n_bits");
    }
    return 0;
}

static int WriteFileHeader(FILE* f, BMP* in_img, BITDATATYPE bit_data_type){

#undef FUNC_CODE
#define FUNC_CODE 0x08

    unsigned char header[BMP_SIZE_FILEHEADER];
    int imgWidth, imgHeight;
    int imageSize;

    int rgbSize;
    int rest_4;

    size_t numb = 0;

    if (fseek(f, 0, SEEK_SET))
        OWN_ERROR_RETURN(0x001, "fseek");

    header[0] = 'B';
    header[1] = 'M';

    imgWidth = in_img->width;
    imgHeight = in_img->height;

    if(bit_data_type == 1)
    {
        imageSize = imgWidth * 4 * imgHeight + BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER;
    }else if(bit_data_type == 2)
    {
        rgbSize = imgWidth * 3;
        rest_4 = rgbSize % 4;
        if (rest_4 > 0)
            rgbSize += 4 - rest_4;
        imageSize = rgbSize * imgHeight + BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER;
    }
    else
        OWN_ERROR_RETURN(0x002, "unknown bit_data_type");
    IntTo4Bytes(imageSize, header + 2);
    IntTo4Bytes(0, header + 6);
    IntTo4Bytes((BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), header + 10);

    //读取bmp head信息
    numb = fwrite(header, BMP_SIZE_FILEHEADER, 1, f);
    if (numb != 1)
        OWN_ERROR_RETURN(0x003, "fwrite");

    return 0;
}

static int WriteFileInfoHead(FILE* f, BMP* in_img, BITDATATYPE bit_data_type){

#undef FUNC_CODE
#define FUNC_CODE 0x09

    unsigned char header[BMP_SIZE_INFOHEADER];
    int imageSize = 0;
    size_t numb = 0;

    if (fseek(f, BMP_SIZE_FILEHEADER, SEEK_SET))
        OWN_ERROR_RETURN(0x001, "fseek");
    // 14- 17
    IntTo4Bytes(BMP_SIZE_INFOHEADER, header);
    // 18 - 21 
    IntTo4Bytes(in_img->width, header + 4);
    // 22 - 25
    IntTo4Bytes(in_img->height, header + 8);
    // 26 - 27
    IntTo2Bytes(1, header + 12);
    // 28 - 29
    if(bit_data_type == 1)
    {
        IntTo2Bytes(32, header + 14);
    }else if(bit_data_type == 2)
    {
        IntTo2Bytes(24, header + 14);
    }
    // 30 - 33
    IntTo4Bytes(0, header + 16);
    // 34 - 37
     if(bit_data_type == 1)
    {
        imageSize = in_img->width * 4 * in_img->height;
    }else if(bit_data_type == 2)
    {
        int rgbSize;
        int rest_4;
        rgbSize = in_img->width * 3;
        rest_4 = rgbSize % 4;
        if (rest_4 > 0)
            rgbSize += 4 - rest_4;
        imageSize = rgbSize * in_img->height;
    }
    IntTo4Bytes(imageSize, header + 20);
    // 38 - 41
    IntTo4Bytes(0, header + 24);
    // 42 - 45
    IntTo4Bytes(0, header + 28);
    // 46 - 49
    IntTo4Bytes(0, header + 32);
    // 50 - 53
    IntTo4Bytes(0, header + 36);

    numb = fwrite(header, BMP_SIZE_INFOHEADER, 1, f);
    if (numb != 1)
        OWN_ERROR_RETURN(0x002, "fwrite");

    return 0;
}

int WriteBMP(const char* file, BMP* in_img, BITDATATYPE bit_data_type){

#undef FUNC_CODE
#define FUNC_CODE 0x0A

    FILE* f;
    int err = 0;

    fopen_fl(&f, file, "wb");

    if (!f) {
        LOGE("Can not open: %s\n", file);
        OWN_ERROR_RETURN(0x001, "fopen_fl");
    }

    if (in_img == NULL)
        OWN_ERROR_RETURN(0x002, "in_img");

    if (0 != (err = WriteFileHeader(f, in_img, bit_data_type))){
        fclose(f);
        ERROR_RETURN(err, "WriteFileHeader");
    }

    if(0 != (err = WriteFileInfoHead(f, in_img, bit_data_type))){
        fclose(f);
        ERROR_RETURN(err, "WriteFileInfoHead");
    }

    if(bit_data_type == 2){
        int rgb_size;
        unsigned char * rgb;
        int y;
        unsigned char* line;
        int rest_4;

        rgb_size = 3 * in_img->width;

        rest_4 = rgb_size % 4;
        if (rest_4 > 0)
            rgb_size += 4 - rest_4;


        rgb = (unsigned char *)malloc(rgb_size * in_img->height);

        if (rgb == NULL)
            OWN_ERROR_RETURN(0x003, "malloc");

        if(in_img->channels == 4)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 4 * y;
                line = rgb + rgb_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line += 3;
                    numb ++;
                }
            }
        }else if(in_img->channels == 3)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 3 * y;
                line = rgb + rgb_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line += 3;
                }
            }
        }
        if (fseek(f, (BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), SEEK_SET)){
            fclose(f);
            OWN_ERROR_RETURN(0x004, "fseek");
        }
        fwrite(rgb, rgb_size * in_img->height, 1, f); 
        fclose(f);
        free(rgb);
    }else if(bit_data_type == 1)
    {
        int rgba_size;
        unsigned char * rgba;
        int y;
        unsigned char* line;
        rgba_size = 4 * in_img->width;


        rgba = (unsigned char *)malloc(rgba_size * in_img->height);

        if (rgba == NULL)
            OWN_ERROR_RETURN(0x005, "malloc");

        if(in_img->channels == 4)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 4 * y;
                line = rgba + rgba_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];//R
                    line[1] = in_img->data[numb++];//G
                    line[0]= in_img->data[numb++];//B
                    line[3]= in_img->data[numb++];//A
                    line += 4;
                }
            }
        }else if(in_img->channels == 3)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 3 * y;
                line = rgba + rgba_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line[3]= 255;
                    line += 4;
                }
            }
        }
        if (fseek(f, (BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), SEEK_SET)){
            fclose(f);
            OWN_ERROR_RETURN(0x006, "fseek");
        }
        fwrite(rgba, rgba_size * in_img->height, 1, f); 
        fclose(f);
        free(rgba);
    }
    else{
        OWN_ERROR_RETURN(0x007, "unknown bit_data_type");
    }
    return 0;
}

int FreeBMP(BMP* img){

#undef FUNC_CODE
#define FUNC_CODE 0x0B

    if(img == NULL){
        return 0;
    }
    if(img->data != NULL){
        free(img->data);
        img->data = NULL;
        img->width = 0;
        img->height = 0;
    }
    return 0;
}

