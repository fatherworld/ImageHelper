#ifndef _LOG_H_FL_
#define _LOG_H_FL_

#include "Config.h"

#if OS_ALVA==OS_Android

#include <jni.h>
#include <android/log.h>

#ifdef AlvaPublic
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG AlvaTag
#else
#ifndef LOG_TAG
#define LOG_TAG "DEFAULT_TAG"
#endif
#endif

#if DebugInfo_Switch == Release_Alva

#define  LOGI(...)  
#define  LOGD(...)  
#define  LOGV(...)  
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGP(...) 
#define  LOGPD(...) 
#define  LOGS(mString)  

#else

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGV(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGP(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGPD(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGS(mString)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, "%s", mString)

#endif //DebugInfo_Switch
#endif //OS_ALVA

#if OS_ALVA==OS_Windows
#include <Windows.h>
#include "stdio_fl.h"

#if DebugInfo_Switch == Release_Alva

#define  LOGI(...)  
#define  LOGD(...)  
#define  LOGV(...)  
#define  LOGE(...)  {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGP(...)  {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "\r%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGPD(...) {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "\n%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGS(mString)  

#else

#define  LOGI(...)  {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGD(...)  {\
                        char t_log_temp_do_not_use[1024] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGV(...)  {\
                        char t_log_temp_do_not_use[1024] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGE(...)  {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGP(...)  {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "\r%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGPD(...) {\
                        char t_log_temp_do_not_use[8192] = {0};      \
                        int t_log_num_do_not_use = sprintf_fl(t_log_temp_do_not_use, 8192, "\n%-40s:", LOG_TAG); \
                        sprintf_fl(&(t_log_temp_do_not_use[t_log_num_do_not_use]), 8192 - t_log_num_do_not_use, __VA_ARGS__); \
                        printf_fl("%s", t_log_temp_do_not_use);OutputDebugStringA(t_log_temp_do_not_use);    \
                    }
#define  LOGS(mString)  {\
                        printf_fl("%s", mString);OutputDebugStringA(mString);    \
                    }
#endif //DebugInfo_Switch
#endif //OS_ALVA

#if OS_ALVA==OS_Linux || OS_ALVA==OS_iOS

#if DebugInfo_Switch == Release_Alva

#define  LOGI(...)  
#define  LOGD(...)  
#define  LOGV(...)  
#define  LOGE(...) printf_fl(__VA_ARGS__);
#define  LOGP(...) 
#define  LOGPD(...) 
#define  LOGS(mString)  

#else

#define  LOGI(...)  printf_fl(__VA_ARGS__);
#define  LOGD(...)  printf_fl(__VA_ARGS__);
#define  LOGV(...)  printf_fl(__VA_ARGS__);
#define  LOGE(...)  printf_fl(__VA_ARGS__);
#define  LOGP(...)  printf_fl(__VA_ARGS__);
#define  LOGPD(...) printf_fl(__VA_ARGS__);
#define  LOGS(mString) printf_fl("%s\n", mString);

#endif //DebugInfo_Switch
#endif //OS_ALVA

#endif //_LOG_H_FL_