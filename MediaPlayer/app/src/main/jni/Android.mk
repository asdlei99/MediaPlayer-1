LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := avjni
LOCAL_SRC_FILES := ./libsrc/FFmpegParameters.cpp
LOCAL_SRC_FILES += ./libsrc/readers/FFmpegAudioReader.cpp
LOCAL_SRC_FILES += ./libsrc/readers/FFmpegVideoReader.cpp
LOCAL_SRC_FILES += ./libsrc/FFmpegAudioReaderFuncs.cpp
LOCAL_SRC_FILES += ./libsrc/FFmpegWrapper.cpp
LOCAL_SRC_FILES += ./libsrc/FFmpegFileHolder.cpp
LOCAL_SRC_FILES += ./libsrc/VideoVectorBuffer.cpp
LOCAL_SRC_FILES += ./libsrc/system/System_routine.cpp
LOCAL_SRC_FILES += avjni.c

LOCAL_CFLAGS    := -D__unix__

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

