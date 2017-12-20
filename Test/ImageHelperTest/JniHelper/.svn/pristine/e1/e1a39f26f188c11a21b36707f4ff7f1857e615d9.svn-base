#include "Config.h"

#include "stdio_fl.h"
#include "string_fl.h"
#include <stdlib.h>

#include "Format.h"

#define LOG_TAG "AndroidHelper"
#include "Log.h"
#include "ErrorTools.h"

#import <Foundation/Foundation.h>

#include "AndroidHelper.h"

#undef FILE_NUM
#define FILE_NUM 0xD100

int Android_GetPackageName(char* outPackageName, int length){

#undef FUNC_CODE
#define FUNC_CODE 0x01

    NSString *app_Name = [[NSBundle mainBundle] bundleIdentifier];
    if (!([app_Name isEqualToString:@""] || !app_Name)){
        const char* appname = [app_Name UTF8String];
        strncpy(outPackageName, appname, length);
        outPackageName[length - 1]='\0';
    }
    
    return 0;
}
int readData(const char *filename, alvaft type, char ** sources, int * fileLengths){

#undef FUNC_CODE
#define FUNC_CODE 0x03

    int fileLength;
    
    size_t ret;

    if (filename == NULL)
        OWN_ERROR_RETURN(0x001, "filename");

    if (type == Alva_Asset){
        LOGE("Alva_Asset is not supported on iOS\n");
        return ERROR_CODE(0x002 | ERROR_OWNER);
    }
    else{
        FILE *file = NULL;
        fopen_fl(&file, filename, "rb");

        if (file == NULL)
            OWN_ERROR_RETURN(0x005, "fopen_fl");

        fseek(file, 0, SEEK_END);
        fileLength = (int)ftell(file);
		*fileLengths = fileLength;
        *sources = (char*)malloc(fileLength + 1);
        rewind(file);
        ret = fread(*sources, fileLength, 1, file);
        (*sources)[fileLength] = '\0';
        if (ret == 0) {
            fclose(file);
            OWN_ERROR_RETURN(0x006, "fread");
        }

        fclose(file);
    }
	return 0;
}
