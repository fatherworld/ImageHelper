#include "Config.h"

#include <jni.h>
#include <android/log.h>
#include <malloc.h>

#include "Format.h"

#define  LOG_TAG "ImageDistortionCorrection_JNI"
#include "Log.h"

#include "AndroidHelper.h"
#include "ImageDistortionCorrection.h"

#ifndef CLASSNAME
#error CLASSNAME was not defined, please define CLASSNAME first like -DCLASSNAME=com_alvasystems_ImageDC
#endif

#define JNIFUNC_NOTUSE2(CLZ, FUN) JNIEXPORT JNICALL Java_##CLZ##_##FUN
#define JNIFUNC_NOTUSE1(CLZ, FUN) JNIFUNC_NOTUSE2(CLZ, FUN)
#define JNIFUNC(FUNCNAME) JNIFUNC_NOTUSE1(CLASSNAME, FUNCNAME)

extern "C" {
    jint
    JNIFUNC(ImageDistortionCorrection)(JNIEnv * env, jobject obj, jbyteArray idata, jint iwidth, jint iheight, jbyteArray odata, jint owidth, jint oheight, jfloatArray dcData, jint dcType){
    	unsigned char* cidata = NULL;
    	unsigned char* codata = NULL;
        float* cDCData = NULL;
        if (idata)
        	cidata = (unsigned char *) env->GetByteArrayElements(idata, 0);

        if (odata)
        	codata = (unsigned char *) env->GetByteArrayElements(odata, 0);

    	if (dcData)
    		cDCData = (float *)env->GetFloatArrayElements(dcData, 0);

        ImageDistortionCorrection(cidata, iwidth, iheight, codata, owidth, oheight, cDCData, (DCType)dcType);

    	if (cidata)
            env->ReleaseByteArrayElements(idata, (jbyte*)cidata, JNI_ABORT);

    	if (odata)
    	    env->ReleaseByteArrayElements(odata, (jbyte*)odata, JNI_ABORT);

    	if (cDCData)
    	    env->ReleaseFloatArrayElements(dcData, (jfloat*)cDCData, JNI_ABORT);

        return 0;
    }
}