#include "Config.h"

#if OS_ALVA == OS_Android
#include <jni.h>
#include <android/log.h>
#include <malloc.h>

#define LOG_TAG "JniLoad"
#include "Log.h"
#include "ErrorTools.h"

#include "JniHolder.h"

#undef FILE_NUM
#define FILE_NUM 0xD200

#ifndef CLASSNAME
#error CLASSNAME was not defined, please define CLASSNAME first like -DCLASSNAME=com_alvasystems_ir_IRJni
#endif

#define JNIFUNC_NOTUSE2(CLZ, FUN) JNIEXPORT JNICALL Java_##CLZ##_##FUN
#define JNIFUNC_NOTUSE1(CLZ, FUN) JNIFUNC_NOTUSE2(CLZ, FUN)
#define JNIFUNC(FUNCNAME) JNIFUNC_NOTUSE1(CLASSNAME, FUNCNAME)

extern "C" {
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){

#undef FUNC_CODE
#define FUNC_CODE 0x01

        setJniHolderInited(false);
        setVM(vm);
        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved){

#undef FUNC_CODE
#define FUNC_CODE 0x02

    }

    void
    JNIFUNC(JniInit)(JNIEnv* env, jobject obj, jobject assetManager){

#undef FUNC_CODE
#define FUNC_CODE 0x03

        setJNIEnv(env);
        setContext(obj);
        setAssetManager(reinterpret_cast<jobject>(env->NewGlobalRef(assetManager)));
        setJniHolderInited(true);
    }
}
#endif