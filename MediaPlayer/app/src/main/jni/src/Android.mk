LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SDL_PATH := ../SDL

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libsrc/threads/OpenThreads/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_PATH)/include


LOCAL_MODULE    := avjni
LOCAL_SRC_FILES += $(SDL_PATH)/src/main/android/SDL_android_main.c
 LOCAL_SRC_FILES += ./sdl_main.cpp
LOCAL_SRC_FILES += ../libsrc/SDLAudioSink.cpp
LOCAL_SRC_FILES += ../libsrc/FFmpegParameters.cpp
LOCAL_SRC_FILES += ../libsrc/readers/FFmpegAudioReader.cpp
LOCAL_SRC_FILES += ../libsrc/readers/FFmpegVideoReader.cpp
LOCAL_SRC_FILES += ../libsrc/FFmpegAudioReaderFuncs.cpp
LOCAL_SRC_FILES += ../libsrc/readers/FFmpegWrapper.cpp
LOCAL_SRC_FILES += ../libsrc/FFmpegFileHolder.cpp
LOCAL_SRC_FILES += ../libsrc/buffers/VideoVectorBuffer.cpp
LOCAL_SRC_FILES += ../libsrc/buffers/AudioBuffer.cpp
LOCAL_SRC_FILES += ../libsrc/system/System_routine.cpp
LOCAL_SRC_FILES += ../libsrc/system/Timer.cpp
LOCAL_SRC_FILES += ../libsrc/system/FFmpegTimer.cpp
 LOCAL_SRC_FILES += ../libsrc/threads/OpenThreads/pthread_src/PThread.c++
LOCAL_SRC_FILES += ../libsrc/threads/OpenThreads/pthread_src/PThreadMutex.c++
LOCAL_SRC_FILES += ../libsrc/threads/OpenThreads/pthread_src/PThreadCondition.c++
 LOCAL_SRC_FILES += ../libsrc/threads/FFmpegRenderThread.cpp
LOCAL_SRC_FILES += ../libsrc/streams/FFmpegStreamer.cpp
LOCAL_SRC_FILES += ../libsrc/streams/FFmpegLibAvStreamImpl.cpp
LOCAL_SRC_FILES += ../libsrc/streams/FFmpegAudioStream.cpp
LOCAL_SRC_FILES += ../libsrc/FFmpegPlayer.cpp
LOCAL_SRC_FILES += ../libsrc/devices/VideoOutputDevice.cpp
LOCAL_SRC_FILES += ../libsrc/devices/VideoOutputDeviceGL.cpp
LOCAL_SRC_FILES += ../libsrc/devices/VideoOutputDeviceSDL.cpp
LOCAL_SRC_FILES += ../libsrc/AudioSinkManager.cpp
LOCAL_SRC_FILES += ../libsrc/SDLMuteAudioSink.cpp
LOCAL_SRC_FILES += ../libsrc/SDLProxiAudioSink.cpp
# LOCAL_SRC_FILES += avjni.c
LOCAL_SRC_FILES += game.c

LOCAL_CFLAGS    := -D__unix__ -DANDROID

# LOCAL_LDLIBS := -llog -ljnigraphics -lz -landroid
LOCAL_LDLIBS += -llog -lz -ljnigraphics -lGLESv2 -lGLESv1_CM

#LOCAL_SHARED_LIBRARIES := libavutil libavfilter libswresample libswscale libavcodec libavformat libavdevice
LOCAL_SHARED_LIBRARIES := libavutil libswresample libswscale libavcodec libavformat libavdevice
LOCAL_SHARED_LIBRARIES += SDL2

include $(BUILD_SHARED_LIBRARY)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
$(call import-module,ffmpeg-2.4.2/android/armv7-a)
else
$(call import-module,ffmpeg-2.4.2/android/arm)
endif

