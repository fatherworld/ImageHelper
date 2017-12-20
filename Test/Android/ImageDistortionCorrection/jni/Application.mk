APP_STL      := gnustl_shared
APP_CPPFLAGS := -frtti -fexceptions -O3 -Ofast -ffast-math -ftree-vectorize -std=gnu++0x -Wno-deprecated -pthread -std=c++11
APP_CFLAGS   := -O3 -Ofast -ffast-math -ftree-vectorize
APP_ABI      := armeabi-v7a arm64-v8a
APP_OPTIM    := release
LOCAL_SHORT_COMMANDS := true
APP_SHORT_COMMANDS := true
