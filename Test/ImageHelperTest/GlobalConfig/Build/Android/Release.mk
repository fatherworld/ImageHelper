#
#
# 此文件使用方法
# 在Android.mk文件中通过以下方法调用
#
# #通过include引用此文件
#
# include $(LOCAL_PATH)/$(Shared_PATH)/GlobalConfig/Build/Android/Release.mk
#
#

LOCAL_CFLAGS    += -DOS_ALVA=OS_Android
LOCAL_CFLAGS    += -DDebugInfo_Switch=Release_Alva 
LOCAL_CFLAGS    += -DAlva_Use_Version=Alva_CL_Version
ifeq ($(TARGET_ARCH),arm)

LOCAL_CFLAGS    += -mfloat-abi=softfp -mfpu=neon

LOCAL_CFLAGS    += -DCPU_ALVA=CPU_ARM32

endif

ifeq ($(TARGET_ARCH),arm64)

LOCAL_CFLAGS    += -DCPU_ALVA=CPU_ARM64

endif

ifeq ($(TARGET_PLATFORM),android-9)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-12)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-13)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-14)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-15)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-16)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-17)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-18)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-19)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_Low_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-21)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_High_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-22)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_High_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-23)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_High_PLATFORM
endif

ifeq ($(TARGET_PLATFORM),android-24)
LOCAL_CFLAGS    += -DAlva_Android_PLATFORM=Alva_High_PLATFORM
endif
