LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

GC_INCLUDE                 := $(LOCAL_PATH)/../../../../../../../trunk/GlobalConfig
ImageHelper_INCLUDE        := $(LOCAL_PATH)/../../../../Core/include
STD_MULTI_PLATFORM_INCLUDE := $(LOCAL_PATH)/../../../../../../../trunk/std-multi-platform/include
JniHelper_INCLUDE          := $(LOCAL_PATH)/../../../../../../../trunk/JniHelper/include/
DT_INCLUDE                 := $(LOCAL_PATH)/../../../../../../../trunk/DebugTools/include/


JniHelper_PATH          := ./../../../../../../../trunk/JniHelper/src
ImageHelper_PATH        := ./../../../../Core/src
STD_MULTI_PLATFORM_PATH := ./../../../../../../../trunk/std-multi-platform/include

LOCAL_MODULE            := ImageDC

IMAGE_HELPER_C_FILES    := ImageDistortionCorrection.c
IMAGE_HELPER_SRC_FILES  := $(addprefix $(ImageHelper_PATH)/, $(IMAGE_HELPER_C_FILES))

JNI_HELPER_C_FILES      := AndroidHelper.c                                     \
                           JniHolder.c                                         \
                           JniLoad.cpp
JNI_HELPER_SRC_FILES    := $(addprefix $(JniHelper_PATH)/, $(JNI_HELPER_C_FILES))

STD_MULTI_PLATFORM_C_FILES := aligned_malloc.c
STD_MULTI_PLATFORM_SRC_FILES := $(addprefix $(STD_MULTI_PLATFORM_PATH)/, $(STD_MULTI_PLATFORM_C_FILES))

ImageDistortionCorrection_JNI_SRC_FILES := ImageDistortionCorrection_Jni.cpp

LOCAL_SRC_FILES	        += $(IMAGE_HELPER_SRC_FILES)                           \
                           $(JNI_HELPER_SRC_FILES)                             \
                           $(STD_MULTI_PLATFORM_SRC_FILES)                     \
                           $(ImageDistortionCorrection_JNI_SRC_FILES)          

LOCAL_C_INCLUDES        := $(GC_INCLUDE)                                       \
                           $(JniHelper_INCLUDE)                                \
                           $(DT_INCLUDE)                                       \
                           $(STD_MULTI_PLATFORM_INCLUDE)                       \
                           $(ImageHelper_INCLUDE)
                           

AlvaDefineRoot          := $(LOCAL_PATH)

include $(AlvaDefineRoot)/AlvaDefines.mk

LOCAL_LDLIBS            := -llog -landroid -lEGL -lGLESv1_CM
LOCAL_STATIC_LIBRARIES  := android_native_app_glue

$(warning $(LOCAL_CFLAGS))

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
