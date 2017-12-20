#include "Config.h"

#define LOG_TAG "JniHolder"
#include "Log.h"
#include "ErrorTools.h"

#include "JniHolder.h"

#undef FILE_NUM
#define FILE_NUM 0xD300

#if OS_ALVA == OS_Android

static JniHolder holder = {0};

JniHolder* getJniHolder(){

#undef FUNC_CODE
#define FUNC_CODE 0x01

    return &holder;
}

void setVM(JavaVM* vm){

#undef FUNC_CODE
#define FUNC_CODE 0x02

    holder.mVM = vm;
}

void setJNIEnv(JNIEnv* env){

#undef FUNC_CODE
#define FUNC_CODE 0x03

    holder.mEnv = env;
}

void setContext(jobject context) {

#undef FUNC_CODE
#define FUNC_CODE 0x04

    holder.mContext = context;
}

void setAssetManager(jobject assetmanager){

#undef FUNC_CODE
#define FUNC_CODE 0x05

    holder.mAssetManager = assetmanager;
}

void setJniHolderInited(int value){

#undef FUNC_CODE
#define FUNC_CODE 0x06

    holder.inited = value;
}

int isJniHolderInited(){

#undef FUNC_CODE
#define FUNC_CODE 0x07

    return holder.inited;
}

#endif