#ifndef _JNI_HOLDER_H_FL_
#define _JNI_HOLDER_H_FL_

#include "Config.h"

#ifdef AlvaPublic
#include "JniHelper_Hide.h"
#endif

#if OS_ALVA==OS_Android

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    JavaVM* mVM;
    JNIEnv* mEnv;
    jobject mContext;
    jobject mAssetManager;
    int inited;
}JniHolder;

JniHolder* getJniHolder();
void setVM(JavaVM* vm);
void setJNIEnv(JNIEnv* env);
void setContext(jobject context);
void setAssetManager(jobject assetmanager);
void setJniHolderInited(int value);
int isJniHolderInited();

#ifdef __cplusplus
}
#endif

#endif //OS_Android

#endif