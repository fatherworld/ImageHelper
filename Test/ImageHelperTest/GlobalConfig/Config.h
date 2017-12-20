#ifndef _ALVA_CONFIG_FL_
#define _ALVA_CONFIG_FL_

//Alva_Role
#define Role_Client   0
#define Role_Server   1

#ifndef Role_ALVA
//Please define Role_ALVA like following: \nAndroid: add LOCAL_CFLAGS := -DRole_ALVA=Role_xx in Android.mk Windows: add /DRole_ALVA=Role_xx in ProjectProperty->C\C++->CommandLine"
#define Role_ALVA Role_Client
#endif

//_Alva_OS_List_
#define OS_Android    0
#define OS_iOS        1
#define OS_Windows    2
#define OS_Linux      3
#define OS_MacOS      4

#ifndef OS_ALVA
#error "OS_ALVA is not defined. Please define OS_ALVA like following: \nAndroid: add LOCAL_CFLAGS := -DOS_ALVA=OS_Android in Android.mk Windows: add /DOS_ALVA=OS_Windows in ProjectProperty->C\C++->CommandLine"
#endif

//_Alva_CPU_
#define CPU_ARM32     0
#define CPU_ARM64     1
#define CPU_x86       2
#define CPU_x64       3
#define CPU_mips      4

#if OS_ALVA == OS_iOS
#undef CPU_ALVA
#ifdef __aarch64__
#define CPU_ALVA CPU_ARM64
#else
#define CPU_ALVA CPU_ARM32
#endif
#endif
#ifndef CPU_ALVA
#error "CPU_ALVA is not defined. Please define CPU_ALVA like following: \nAndroid: add LOCAL_CFLAGS := -DCPU_ALVA=CPU_ARM in Android.mk Windows: add /DCPU_ALVA=CPU_x86 in ProjectProperty->C\C++->CommandLine"
#endif

//_DUMP_SWITCH_List_
#define Dump_On       1
#define Dump_Off      0

#ifndef DUMP_SWITCH
#define DUMP_SWITCH Dump_On
#endif

//flow track(based on log)
#define FLOW_TRACK_ON   1
#define FLOW_TRACK_OFF  0

#ifndef FLOW_TRACK_SWITCH
#define FLOW_TRACK_SWITCH FLOW_TRACK_ON
#endif

//performace checker
#define AlvaTime_All        (0xFFFFFFFF)
#define AlvaTime_TotalPerf  (0x1 << 2)
#define AlvaTime_Function   (0x1 << 1)
#define AlvaTime_OFF        0

#ifndef AlvaTime
#define AlvaTime AlvaTime_OFF
#endif

//_DebugInfo(Compile Mode)_
#define Release_Alva  0
#define Debug_Alva    1

#ifndef DebugInfo_Switch
#pragma message "DebugInfo_Switch is not defined. Current is in Debug mode. You can change to Release Mode like following: \nAndroid: add LOCAL_CFLAGS := -DDebugInfo_Switch=Release_Alva in Android.mk Windows: add /DDebugInfo_Switch=Release_Alva in ProjectProperty->C\\C++->CommandLine"
#define DebugInfo_Switch Debug_Alva
#endif

#if DebugInfo_Switch==Release_Alva
#undef DUMP_SWITCH
#define DUMP_SWITCH Dump_Off

#undef FLOW_TRACK_SWITCH
#define FLOW_TRACK_SWITCH FLOW_TRACK_OFF
#endif

#if DUMP_SWITCH
#pragma message("Info(No harm): Dump is turned on.")
#endif

#if FLOW_TRACK_SWITCH
#pragma message("Info(No harm): Flow_Track is turned on.")
#endif

//implemention
#define Alva_CL_Version 0
#define Alva_GL_Version 1
#define Alva_C_Version  2

#ifndef Alva_Use_Version
#pragma message "Alva_Use_Version is not defined. Current is in CL_Version mode. You can change to GL_Version Mode like following: \nAndroid: add LOCAL_CFLAGS := -DAlva_Use_Version=Alva_GL_Version in Android.mk Windows: add /DAlva_Use_Version=Alva_GL_Version in ProjectProperty->C\\C++->CommandLine"
#define Alva_Use_Version Alva_CL_Version
#endif

#if OS_ALVA == OS_Windows
#ifdef _USRDLL
#ifdef DLL_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif//DLL_EXPORTS
#else
#define EXPORT_API
#endif//_USRDLL
#endif//OS_ALVA
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define EXPORT_API
#endif//OS_ALVA

#if OS_ALVA == OS_Android
#define Alva_Low_PLATFORM 0
#define Alva_High_PLATFORM 1
#define Alva_None_PLATFORM 2

#ifndef Alva_Android_PLATFORM
#define  Alva_Android_PLATFORM Alva_Low_PLATFORM
#endif

#endif
#endif
