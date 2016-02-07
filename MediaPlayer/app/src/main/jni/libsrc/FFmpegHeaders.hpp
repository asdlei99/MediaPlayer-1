#ifndef HEADER_GUARD_FFMPEG_HEADERS_H
#define HEADER_GUARD_FFMPEG_HEADERS_H

// Solved by modify building the ffmpeg : CXXFLAGS=-D__STDC_CONSTANT_MACROS
// See: https://github.com/ffms/ffms2/issues/11
// #define __STDC_CONSTANT_MACROS
// #define UINTMAX_C  UINT64_C
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
#endif


#define JRFFMPEG_ABLE_PLANAR_AUDIO
#define USE_SWRESAMPLE
#define USE_SWSCALE

#ifdef __cplusplus
extern "C" {
#endif

#define FF_API_OLD_SAMPLE_FMT 0
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


#ifndef AV_CH_LAYOUT_STEREO
	#include <libavutil/channel_layout.h> // Appears to be needed specifically for versions on some Linux distros
#endif

#include <libavdevice/avdevice.h>

#include <libavutil/mathematics.h>
#include <libavutil/parseutils.h>
#include <libavutil/pixdesc.h>

#ifdef USE_SWSCALE
    #include <libswscale/swscale.h>
#endif


#if LIBAVUTIL_VERSION_INT <  AV_VERSION_INT(50,38,0)
#define AV_SAMPLE_FMT_NONE SAMPLE_FMT_NONE
#define AV_SAMPLE_FMT_U8   SAMPLE_FMT_U8
#define AV_SAMPLE_FMT_S16  SAMPLE_FMT_S16
#define AV_SAMPLE_FMT_S32  SAMPLE_FMT_S32
#define AV_SAMPLE_FMT_FLT  SAMPLE_FMT_FLT
#define AV_SAMPLE_FMT_DBL  SAMPLE_FMT_DBL
#define AV_SAMPLE_FMT_NB   SAMPLE_FMT_NB
#endif

// See: https://gitorious.org/libav/libav/commit/8889cc4f5b767b323901115a92318a024336e2a1
// "Add planar sample formats and av_sample_fmt_is_planar() to samplefmt.h."
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51, 17, 0)
    #define JRFFMPEG_ABLE_PLANAR_AUDIO
#endif
// See: https://gitorious.org/libav/dondiego-libav/commit/30223b3bf2ab1c55499d3d52a244221d24fcc784
// "deprecate the audio resampling API."
// But: Existence of swresample does not depends on version of avcodec.
// By analogue with USE_SWSCALE (which defined during preparing solution), and example from:
// https://gitorious.org/neutrino-hd/martiis-libstb-hal/commit/8a68eb3f1522642d5318abb2c75b86285f1ae87e
// using of swresample delegate to definition of USE_SWRESAMPLE
// So, to use modern way (if available) of resampling, we should modify CMake-environment before preparing solution.
// tips:
// 1. Modify "./src/CMakeModules/FindFFmpeg.cmake"
//
// add follow line:
//
// FFMPEG_FIND(LIBSWRESAMPLE  swresample  swresample.h)
//
// after block of lines:
//
// FFMPEG_FIND(LIBAVFORMAT avformat avformat.h)
// FFMPEG_FIND(LIBAVDEVICE avdevice avdevice.h)
// FFMPEG_FIND(LIBAVCODEC  avcodec  avcodec.h)
// FFMPEG_FIND(LIBAVUTIL   avutil   avutil.h)
// FFMPEG_FIND(LIBSWSCALE  swscale  swscale.h)  # not sure about the header to look for here.
//
// 2. Modify "CMakeLists.txt" of plugin, similar to swscale and targeted to definition of USE_SWRESAMPLE
//    add follow lines:
//
//IF(FFMPEG_LIBSWRESAMPLE_FOUND)
//
//    INCLUDE_DIRECTORIES( ${FFMPEG_LIBSWRESAMPLE_INCLUDE_DIRS} ${FFMPEG_LIBSWRESAMPLE_INCLUDE_DIRS}/libswresample )
//
//    ADD_DEFINITIONS(-DUSE_SWRESAMPLE)
//
//    SET(TARGET_EXTERNAL_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWRESAMPLE_LIBRARIES})
//
//ENDIF()
#ifdef USE_SWRESAMPLE
    #include <libswresample/swresample.h>
#endif

// Changes for FFMpeg version greater than 0.6
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
#define CODEC_TYPE_AUDIO AVMEDIA_TYPE_AUDIO
#define CODEC_TYPE_VIDEO AVMEDIA_TYPE_VIDEO
#endif

#ifdef AVERROR
#define AVERROR_IO AVERROR(EIO)
#define AVERROR_NUMEXPECTED AVERROR(EDOM)
#define AVERROR_NOMEM AVERROR(ENOMEM)
#define AVERROR_NOFMT AVERROR(EILSEQ)
#define AVERROR_NOTSUPP AVERROR(ENOSYS)
#define AVERROR_NOENT AVERROR(ENOENT)
#endif

#if LIBAVCODEC_VERSION_MAJOR < 56
   #define AV_CODEC_ID_NONE CODEC_ID_NONE
#endif

#if LIBAVCODEC_VERSION_MAJOR >= 53 || \
    (LIBAVCODEC_VERSION_MAJOR==52 && LIBAVCODEC_VERSION_MINOR>=30) || \
    (LIBAVCODEC_VERSION_MAJOR==52 && LIBAVCODEC_VERSION_MINOR==20 && LIBAVCODEC_VERSION_MICRO >= 1)
    #define USE_AV_LOCK_MANAGER
#endif

#if LIBAVCODEC_VERSION_MAJOR >= 55
    #define JRFFMPEG_ALLOC_FRAME     av_frame_alloc
    #define JRFFMPEG_FREE_FRAME      av_frame_free
#else
    #define JRFFMPEG_ALLOC_FRAME     avcodec_alloc_frame
    #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 59, 100)
        #define JRFFMPEG_FREE_FRAME  avcodec_free_frame
    #else
        #define JRFFMPEG_FREE_FRAME  av_freep
    #endif
#endif

#ifdef __cplusplus
} // extern "C" {
#endif

#ifdef __cplusplus
namespace JAZZROS
{
    AVRational get_time_base_q(void);
    const double now_micros(void);
}
#endif

#endif // HEADER_GUARD_FFMPEG_HEADERS_H
