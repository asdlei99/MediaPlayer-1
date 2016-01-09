LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := avjni
LOCAL_SRC_FILES := ./reader/FFmpegParameters.cpp
LOCAL_SRC_FILES += ./reader/FFmpegAudioReader.cpp
LOCAL_SRC_FILES += ./reader/FFmpegAudioReaderFuncs.cpp 
LOCAL_SRC_FILES += avjni.c

# LOCAL_LDLIBS := -llog -ljnigraphics -lz -landroid
LOCAL_LDLIBS += -llog -lz -ljnigraphics
#LOCAL_SHARED_LIBRARIES := libavutil libavfilter libswresample libswscale libavcodec libavformat libavdevice
LOCAL_SHARED_LIBRARIES := libavutil libswresample libswscale libavcodec libavformat libavdevice

include $(BUILD_SHARED_LIBRARY)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
$(call import-module,ffmpeg-2.4.2/android/armv7-a)
else
$(call import-module,ffmpeg-2.4.2/android/arm)
endif

