#include "Config.h"

#include "stdio_fl.h"
#include "string_fl.h"
#include <stdlib.h>

#include "Format.h"

#define LOG_TAG "AndroidHelper"
#include "Log.h"
#include "ErrorTools.h"

#if OS_ALVA == OS_Android
#include <jni.h>
#include <android/native_activity.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include "JniHolder.h"
#endif
#if OS_ALVA == OS_Windows
#include <Windows.h>
#endif

#include "AndroidHelper.h"

#undef FILE_NUM
#define FILE_NUM 0xD100

// --------------------------------------------------------------------
// C code snippet: helper function to get package name from Java
// --------------------------------------------------------------------
int Android_GetPackageName(char* outPackageName, int length){

#undef FUNC_CODE
#define FUNC_CODE 0x01

#if OS_ALVA == OS_Android
    JniHolder* holder = getJniHolder();

    JavaVM* vm = holder->mVM;
    JNIEnv* env = holder->mEnv;

    jclass activityThread = (*env)->FindClass(env, "android/app/ActivityThread");

    jmethodID currentActivityThread = (*env)->GetStaticMethodID(env, activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");

    jobject at = (*env)->CallStaticObjectMethod(env, activityThread, currentActivityThread);

    jmethodID getApplication = (*env)->GetMethodID(env, activityThread, "getApplication", "()Landroid/app/Application;");

    jobject context = (*env)->CallObjectMethod(env, at, getApplication);

    //note: we need to attach dalvik VM to current thread, as it is not main thread
    if((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) < 0)
        (*vm)->AttachCurrentThread(vm, &env, NULL);

    //get package name from Activity Class(context)
    jclass android_content_Context = (*env)->GetObjectClass(env, context);
    jmethodID midGetPackageName = (*env)->GetMethodID(env, android_content_Context, "getPackageName", "()Ljava/lang/String;");
    jstring PackageName= (jstring)(*env)->CallObjectMethod(env, context, midGetPackageName);

    if(PackageName != NULL) {
        // get UTF8 string & copy to dest
        const char* charBuff = (*env)->GetStringUTFChars(env, PackageName, NULL);
        strncpy(outPackageName, charBuff, length);
        outPackageName[length - 1]='\0';

        (*env)->ReleaseStringUTFChars(env, PackageName, charBuff);
        (*env)->DeleteLocalRef(env, PackageName);
    }
    (*env)->DeleteLocalRef(env, android_content_Context);
    (*env)->DeleteLocalRef(env, context);
    (*env)->DeleteLocalRef(env, activityThread);
    (*env)->DeleteLocalRef(env, at);
  
    return 0;
#endif //OS_ALVA
#if OS_ALVA == OS_Windows
    wchar_t mwPath[2048] = { 0 };
    wchar_t *start = NULL, *end = NULL;

    GetModuleFileNameW(NULL, mwPath, 2048);
    start = wcsrchr(mwPath, L'\\');
    end = wcsrchr(mwPath, L'.');

    *end = L'\0';

    WideCharToMultiByte(CP_ACP, 0, start + 1, -1, outPackageName, length, NULL, NULL);

    return 0;
#endif
}

#if OS_ALVA == OS_Android
char* Jstring2CStr(jstring jstr){

#undef FUNC_CODE
#define FUNC_CODE 0x02

	char* rtn = NULL;

    JniHolder* holder = getJniHolder();
    JNIEnv* env = holder->mEnv;

    jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jstring strencode = (*env)->NewStringUTF(env, "UTF-8");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray)((*env)->CallObjectMethod(env, jstr, mid, strencode));	//calling String .getByte("UTF-8")
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	if(alen > 0){
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    (*env)->DeleteLocalRef(env, strencode);
	return rtn;
}
#endif //OS_ALVA

int readData(const char *filename, alvaft type, char ** sources, int * fileLengths){

#undef FUNC_CODE
#define FUNC_CODE 0x03

    int fileLength;
    
    size_t ret;

    if (filename == NULL)
        OWN_ERROR_RETURN(0x001, "filename");

    if (type == Alva_Asset)
    {
#if OS_ALVA == OS_Android
        JniHolder* holder = getJniHolder();
        JNIEnv* env = holder->mEnv;
        jobject assetManager = holder->mAssetManager;

        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

        if (mgr == NULL)
            OWN_ERROR_RETURN(0x002, "fromJava");

        AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_UNKNOWN);
        if (asset == NULL)
            OWN_ERROR_RETURN(0x003, "open");

        fileLength = AAsset_getLength(asset);
		*fileLengths = fileLength;
        *sources = (char*)malloc(fileLength + 1);
        ret = AAsset_read(asset, *sources, fileLength);
        (*sources)[fileLength] = '\0';
        if (ret <= 0)
            OWN_ERROR_RETURN(0x004, "read");

        AAsset_close(asset);
#endif
    }
    else
    {
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
