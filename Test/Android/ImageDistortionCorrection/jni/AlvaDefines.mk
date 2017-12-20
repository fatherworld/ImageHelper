LOCAL_CFLAGS    := -DOS_ALVA=OS_Android
LOCAL_CFLAGS    += -DDUMP_SWITCH=Dump_Off
LOCAL_CFLAGS    += -DFLOW_TRACK_SWITCH=FLOW_TRACK_OFF
LOCAL_CFLAGS    += -DDebugInfo_Switch=Release_Alva 
LOCAL_CFLAGS    += -DAlva_Use_Version=Alva_CL_Version
LOCAL_CFLAGS    += -DCLASSNAME=com_alvasystems_ImageDC
LOCAL_CFLAGS    += -DIR
LOCAL_CFLAGS    += -DAlvaTime=AlvaTime_Function

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS    += -mfloat-abi=softfp -mfpu=neon
LOCAL_CFLAGS    += -DCPU_ALVA=CPU_ARM32
endif
ifeq ($(TARGET_ARCH),arm64)
LOCAL_CFLAGS    += -DCPU_ALVA=CPU_ARM64
endif