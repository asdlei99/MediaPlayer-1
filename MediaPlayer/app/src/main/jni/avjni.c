#include <jni.h>
#include <assert.h>
/*
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
 */
#include <android/log.h>
#include <android/bitmap.h>

#include "libsrc/FFmpegHeaders.hpp"


#ifdef USE_AV_LOCK_MANAGER
    extern int (*pfLockMgr)(void **mutex, enum AVLockOp op);
    extern void initializePfLockMgr(void);
#endif // USE_AV_LOCK_MANAGER

/**
 * Declare JAZZROS V/A API
 */
const int   gAudioReaderInit();
const int   gAudioReaderOpen(const char * mfileName);
void        gAudioReaderClose();
const int   gAudioReaderGetSamples(unsigned long * msTime,
                                 unsigned short channelsNb,
                                 const enum AVSampleFormat * output_sampleFormat,
                                 unsigned short * sample_rate,
                                 unsigned long * samplesNb,
                                 unsigned char * bufSamples,
                                 const double * max_avail_time_micros);

//
// ros:
//
/* in bytes */
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenFromFile(JNIEnv* env, jobject thiz, jstring mediafile);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenFromRawStream(JNIEnv* env, jobject thiz, jbyteArray mediaframe);
JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeClose(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenAudio(JNIEnv* env, jobject thiz, jbyteArray audioframe, jintArray audioframelength);
JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeCloseAudio(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenVideo(JNIEnv* env, jobject thiz, jobject bitmap);
JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeCloseVideo(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeDecodeFrameFromFile(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeDecodeFrameFromRawStream(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegOpenAudio(JNIEnv* env, jobject thiz, jstring mediafile, jbyteArray audioframe, jintArray audioframelength, jint channelsNb, jint sampleFmt, jint sampleRate, jint sampleNb);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegDecodeAudio(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegCloseAudio(JNIEnv* env, jobject thiz);
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeUpdateBitmap(JNIEnv* env, jobject thiz);
JNIEXPORT jdouble JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeGetFps(JNIEnv* env, jobject thiz);

#define AUDIO_DATA_ID   1
#define VIDEO_DATA_ID   2


AVFormatContext*    gFormatCtx;

//video
AVCodecContext*     gVideoCodecCtx;
int                 gVideoStreamIdx;
AVFrame*            gVideoFrame;
jobject             gBitmapRef;
void*               gBitmapRefPixelBuffer;
AndroidBitmapInfo   gAbi;
struct SwsContext*  gSwsContext;
long                m_bytesRemaining;

//audio
AVCodecContext*     gAudioCodecCtx;
int                 gAudioStreamIdx;
jbyteArray          gAudioFrameRef; //reference to a java variable
char*               gAudioFrameRefBuffer;
int                 gAudioFrameRefBufferMaxSize;
jintArray           gAudioFrameDataLengthRef; //reference to a java variable
int*                gAudioFrameDataLengthRefBuffer;
//
jbyteArray          gFFmpegAudioFrameRef; //reference to a java variable
char*               gFFmpegAudioFrameRefBuffer;
jintArray           gFFmpegAudioFrameDataLengthRef; //reference to a java variable
int*                gFFmpegAudioFrameDataLengthRefBuffer;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "JNI_OnLoad()");

    gFormatCtx = NULL;

    //video
    gVideoCodecCtx = NULL;
    gVideoStreamIdx = -1;
    gVideoFrame = NULL;
    gBitmapRef = NULL;
    gBitmapRefPixelBuffer = NULL;
    gSwsContext = NULL;
    memset(&gAbi, 0, sizeof(gAbi));
    m_bytesRemaining = 0;

    //audio
    gAudioCodecCtx = NULL;
    gAudioStreamIdx = -1;
    gAudioFrameRef = NULL;
    gAudioFrameRefBuffer = NULL;
    gAudioFrameRefBufferMaxSize = 0;
    gAudioFrameDataLengthRef = NULL;
    gAudioFrameDataLengthRefBuffer = NULL;

    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenFromFile(JNIEnv* env, jobject thiz, jstring mediafile)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeOpenFromFile()");

#ifdef USE_AV_LOCK_MANAGER
    // enable thread locking
    initializePfLockMgr();
    av_lockmgr_register(pfLockMgr);
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "av_lockmgr_register()");
#else
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "av_lockmgr_register() not supported");
#endif

//    avcodec_init();
    av_register_all();
    avcodec_register_all();

    const char* mfileName = (*env)->GetStringUTFChars(env, mediafile, 0);
    if (mfileName == 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to retrieve media file name");
        return -1;
    }

    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "opening %s", mfileName);

    int result = avformat_open_input(&gFormatCtx, mfileName, NULL, NULL);
    (*env)->ReleaseStringUTFChars(env, mediafile, mfileName); //always release the java string reference
    if (result != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avformat_open_input() failed");
        return -2;
    }

    if (avformat_find_stream_info(gFormatCtx, NULL) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avformat_find_stream_info() failed");
        return -3;
    }

    return 0;
}

JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeClose(JNIEnv* env, jobject thiz)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeClose()");

    Java_com_jazzros_ffmpegtest_AVThread_nativeCloseAudio(env, thiz);
    Java_com_jazzros_ffmpegtest_AVThread_nativeCloseVideo(env, thiz);

    if (gFormatCtx)
    {
        //ros: av_close_input_file(gFormatCtx);
//    	avformat_close_input(gFormatCtx);

// see: https://gitorious.org/ffmpeg/sastes-ffmpeg/commit/5266045
// "add avformat_close_input()."
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 17, 0)
        avformat_close_input(&gFormatCtx);
#else
        av_close_input_file(gFormatCtx);
#endif

        gFormatCtx = NULL;
    }
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenAudio(JNIEnv* env, jobject thiz, jbyteArray audioframe, jintArray audioframelength)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeOpenAudio()");

    if (gAudioFrameRef)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "call nativeCloseAudio() before calling this function");
        return -1;
    }

    if ((*env)->IsSameObject(env, audioframe, NULL))
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "invalid arguments");
        return -2;
    }

    //audio frame buffer
    gAudioFrameRef = (*env)->NewGlobalRef(env, audioframe); //lock the array preventing the garbage collector from destructing it
    if (gAudioFrameRef == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "NewGlobalRef() for audioframe failed");
        return -3;
    }

    jboolean test;
    gAudioFrameRefBuffer = (*env)->GetByteArrayElements(env, gAudioFrameRef, &test);
    if (gAudioFrameRefBuffer == 0 || test == JNI_TRUE)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to get audio frame reference or reference copied");
        return -4;
    }

    gAudioFrameRefBufferMaxSize = (*env)->GetArrayLength(env, gAudioFrameRef);
    if (gAudioFrameRefBufferMaxSize < AVCODEC_MAX_AUDIO_FRAME_SIZE)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to read or incorrect buffer length: %d", gAudioFrameRefBufferMaxSize);
        return -5;
    }

    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "buffer length: %d", gAudioFrameRefBufferMaxSize);

    //audio frame data size
    gAudioFrameDataLengthRef = (*env)->NewGlobalRef(env, audioframelength); //lock the variable preventing the garbage collector from destructing it
    if (gAudioFrameDataLengthRef == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "NewGlobalRef() for audioframelength failed");
        return -6;
    }

    gAudioFrameDataLengthRefBuffer = (*env)->GetIntArrayElements(env, gAudioFrameDataLengthRef, &test);
    if (gAudioFrameDataLengthRefBuffer == 0 || test == JNI_TRUE)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to get audio data length reference or reference copied");
        return -7;
    }

    int audioDataLength = (*env)->GetArrayLength(env, gAudioFrameDataLengthRef);
    if (audioDataLength != 1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to read or incorrect size of the audio data length reference: %d", audioDataLength);
        return -8;
    }

    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "size of the audio data length reference: %d", audioDataLength);

    //audio stream
    int i;
    int audioStreamIdx = -1;
    for (i = 0; i < gFormatCtx->nb_streams && audioStreamIdx == -1; ++i)
    {
        //ros: if(gFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
    	if(gFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
            audioStreamIdx = i;
    }

    if (audioStreamIdx == -1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "audio stream not found");
        return -9;
    }

    gAudioCodecCtx = gFormatCtx->streams[audioStreamIdx]->codec;
    AVCodec* audioCodec = avcodec_find_decoder(gAudioCodecCtx->codec_id);
    if (!audioCodec)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_find_decoder() failed to find audio decoder");
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "Audio CodecID = %d", gAudioCodecCtx->codec_id);
        return -10;
    }

    if (avcodec_open2(gAudioCodecCtx, audioCodec, NULL) != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_open2() failed");
        return -11;
    }

    //all good, set index so that nativeProcess() can now recognise the audio stream
    gAudioStreamIdx = audioStreamIdx;
//ros:
    if (av_sample_fmt_is_planar(gAudioCodecCtx->sample_fmt) != 0)
    {
        __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "Audio is Planar");
    }

    return 0;
}

JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeCloseAudio(JNIEnv* env, jobject thiz)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeCloseAudio()");

    if (gAudioCodecCtx)
    {
        avcodec_close(gAudioCodecCtx);
        gAudioCodecCtx = NULL;
    }

    if (gAudioFrameRef)
    {
        if (gAudioFrameRefBuffer)
        {
            (*env)->ReleaseByteArrayElements(env, gAudioFrameRef, gAudioFrameRefBuffer, 0);
            gAudioFrameRefBuffer = NULL;
        }
        (*env)->DeleteGlobalRef(env, gAudioFrameRef);
        gAudioFrameRef = NULL;
    }
    gAudioFrameRefBufferMaxSize = 0;

    if (gAudioFrameDataLengthRef)
    {
        if (gAudioFrameDataLengthRefBuffer)
        {
            (*env)->ReleaseIntArrayElements(env, gAudioFrameDataLengthRef, gAudioFrameDataLengthRefBuffer, 0);
            gAudioFrameDataLengthRefBuffer = NULL;
        }
        (*env)->DeleteGlobalRef(env, gAudioFrameDataLengthRef);
        gAudioFrameDataLengthRef = NULL;
    }

    gAudioStreamIdx = -1;
}

void releaseBitmap (JNIEnv* env, jobject thiz)
{
    if (gBitmapRef)
    {
        if (gBitmapRefPixelBuffer)
        {
            AndroidBitmap_unlockPixels(env, gBitmapRef);
            gBitmapRefPixelBuffer = NULL;
        }
        (*env)->DeleteGlobalRef(env, gBitmapRef);
        gBitmapRef = NULL;
    }
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeSetBitmapBuffer(JNIEnv* env, jobject thiz, jobject bitmap)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeSetBitmapBuffer()");

    if ((*env)->IsSameObject(env, bitmap, NULL))
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "invalid arguments");
        return -2;
    }
    // Do not forget release previous instance of resources
    releaseBitmap (env, thiz);

    gBitmapRef = (*env)->NewGlobalRef(env, bitmap); //lock the bitmap preventing the garbage collector from destructing it
    if (gBitmapRef == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "NewGlobalRef() failed");
        return -4;
    }

    int result = AndroidBitmap_lockPixels(env, gBitmapRef, &gBitmapRefPixelBuffer);
    if (result != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "AndroidBitmap_lockPixels() failed with %d", result);
        gBitmapRefPixelBuffer = NULL;
        return -5;
    }

    if (AndroidBitmap_getInfo(env, gBitmapRef, &gAbi) != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "AndroidBitmap_getInfo() failed");
        return -6;
    }

    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "bitmap width: %d", gAbi.width);
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "bitmap height: %d", gAbi.height);
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "bitmap stride: %d", gAbi.stride);
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "bitmap format: %d", gAbi.format);
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "bitmap flags: %d", gAbi.flags);

    return 0;
}


JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeOpenVideo(JNIEnv* env, jobject thiz, jobject bitmap)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeOpenVideo()");

    if (gVideoFrame)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "call nativeCloseVideo() before calling this function");
        return -1;
    }

    gVideoFrame = JRFFMPEG_ALLOC_FRAME();

    if (gVideoFrame == 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_alloc_frame() failed");
        return -3;
    }
    Java_com_jazzros_ffmpegtest_AVThread_nativeSetBitmapBuffer(env, thiz, bitmap);

    int i;
    int videoStreamIdx = -1;
    for (i = 0; i < gFormatCtx->nb_streams && videoStreamIdx == -1; ++i)
    {
        //ros: if(gFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
    	if(gFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            videoStreamIdx = i;
    }

    if (videoStreamIdx == -1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "video stream not found");
        return -7;
    }

    gVideoCodecCtx = gFormatCtx->streams[videoStreamIdx]->codec;
    AVCodec* videoCodec = avcodec_find_decoder(gVideoCodecCtx->codec_id);
    if (!videoCodec)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_find_decoder() failed to find decoder");
        return -8;
    }

    if (avcodec_open2(gVideoCodecCtx, videoCodec, NULL) != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_open2() failed");
        return -9;
    }

    //all good, set index so that nativeProcess() can now recognise the video stream
    gVideoStreamIdx = videoStreamIdx;
    return 0;
}

JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeCloseVideo(JNIEnv* env, jobject thiz)
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeCloseVideo()");

    if (gVideoCodecCtx)
    {
        avcodec_close(gVideoCodecCtx);
        gVideoCodecCtx = NULL;
    }

    sws_freeContext(gSwsContext);
    gSwsContext = NULL;

    releaseBitmap(env, thiz);

    gVideoStreamIdx = -1;

    JRFFMPEG_FREE_FRAME (& gVideoFrame);
    gVideoFrame = NULL;

    memset(&gAbi, 0, sizeof(gAbi));
}

int decodeFrameFromPacket(AVPacket* aPacket)
{
    if (aPacket->stream_index == gVideoStreamIdx)
    {
        m_bytesRemaining = aPacket->size;
        int frameFinished = 0;
        // Work on the current packet until we have decoded all of it
        while (m_bytesRemaining > 0)
        {
            int byteDecoded = avcodec_decode_video2(gVideoCodecCtx, gVideoFrame, &frameFinished, aPacket);
            if (byteDecoded < 0)
            {
                __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "avcodec_decode_video2() decoded no frame");
                return -1;
            }
            m_bytesRemaining -= byteDecoded;
            if (frameFinished)
            {
                 return VIDEO_DATA_ID;
            }
            __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "avcodec_decode_video2() should continue decoding");
        }

        return frameFinished ? VIDEO_DATA_ID : -1;
    }

    if (aPacket->stream_index == gAudioStreamIdx)
    {
        return AUDIO_DATA_ID;
    }

    return 0;
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeDecodeFrameFromFile(JNIEnv* env, jobject thiz)
{
    AVPacket packet;
    memset(&packet, 0, sizeof(packet)); //make sure we can safely free it

    int i;
    for (i = 0; i < gFormatCtx->nb_streams; ++i)
    {
        // av_init_packet(&packet);
        if (av_read_frame(gFormatCtx, &packet) != 0)
        {
            __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "av_read_frame() failed");
            return -1;
        }
        if (packet.stream_index != gVideoStreamIdx)
        {
            av_free_packet(&packet);
            continue;
        }

        int ret = decodeFrameFromPacket(&packet);
//        av_free_packet(&packet);
        if (ret != 0) //an error or a frame decoded
            return ret;
    }

    return 0;
}


jint gChannelsNb;
jint gSampleFmt;
jint gSampleRate;
jint gSampleNb;
JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegOpenAudio(JNIEnv* env, jobject thiz, jstring mediafile, jbyteArray audioframe, jintArray audioframelength, jint channelsNb, jint sampleFmt, jint sampleRate, jint sampleNb )
{
    __android_log_print(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", "nativeFFmpegOpenAudio()");

    if ((*env)->IsSameObject(env, audioframe, NULL))
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "invalid arguments");
        return -2;
    }

    //audio frame buffer
    gFFmpegAudioFrameRef = (*env)->NewGlobalRef(env, audioframe); //lock the array preventing the garbage collector from destructing it
    if (gFFmpegAudioFrameRef == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "NewGlobalRef() for audioframe failed");
        return -3;
    }

    jboolean test;
    gFFmpegAudioFrameRefBuffer = (*env)->GetByteArrayElements(env, gFFmpegAudioFrameRef, &test);
    if (gFFmpegAudioFrameRefBuffer == 0 || test == JNI_TRUE)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to get audio frame reference or reference copied");
        return -4;
    }
    //audio frame data size
    gFFmpegAudioFrameDataLengthRef = (*env)->NewGlobalRef(env, audioframelength); //lock the variable preventing the garbage collector from destructing it
    if (gFFmpegAudioFrameDataLengthRef == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "NewGlobalRef() for audioframelength failed");
        return -6;
    }

    gFFmpegAudioFrameDataLengthRefBuffer = (*env)->GetIntArrayElements(env, gFFmpegAudioFrameDataLengthRef, &test);
    if (gFFmpegAudioFrameDataLengthRefBuffer == 0 || test == JNI_TRUE)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to get audio data length reference or reference copied");
        return -7;
    }

    const char* mfileName = (*env)->GetStringUTFChars(env, mediafile, 0);
    if (mfileName == 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "failed to retrieve media file name");
        return -1;
    }

    gAudioReaderInit();
    if ( gAudioReaderOpen(mfileName) < 0)
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "FFmpegAudioReader cannot open file");

    gChannelsNb = channelsNb;
    gSampleFmt  = sampleFmt;
    gSampleRate = sampleRate;
    gSampleNb   = sampleNb;

    return 0;
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegDecodeAudio(JNIEnv* env, jobject thiz)
{
    unsigned long msTime;
    unsigned short channelsNb = gChannelsNb;
    const enum AVSampleFormat output_sampleFormat = (enum AVSampleFormat)gSampleFmt;//AV_SAMPLE_FMT_S16;
    unsigned short sample_rate = gSampleRate;
    unsigned long samplesNb = gSampleNb;
    unsigned char * bufSamples = gFFmpegAudioFrameRefBuffer;
    const double max_avail_time_micros = 0;

    const int err = gAudioReaderGetSamples(& msTime,
                                         channelsNb,
                                         & output_sampleFormat,
                                         & sample_rate,
                                         & samplesNb,
                                         bufSamples,
                                         & max_avail_time_micros);
    gFFmpegAudioFrameDataLengthRefBuffer[0] = samplesNb * channelsNb * av_get_bytes_per_sample (output_sampleFormat);
    return err;
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AudioThread_nativeFFmpegCloseAudio(JNIEnv* env, jobject thiz)
{
    gAudioReaderClose();
    return 0;
}

JNIEXPORT jint JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeUpdateBitmap(JNIEnv* env, jobject thiz)
{
    //ros: gSwsContext = sws_getCachedContext(gSwsContext, gVideoCodecCtx->width, gVideoCodecCtx->height, gVideoCodecCtx->pix_fmt, gAbi.width, gAbi.height, AV_PIX_FMT_RGB565LE, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    gSwsContext = sws_getCachedContext(gSwsContext, gVideoCodecCtx->width, gVideoCodecCtx->height, gVideoCodecCtx->pix_fmt, gAbi.width, gAbi.height, AV_PIX_FMT_RGB565LE, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (gSwsContext == 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "sws_getCachedContext() failed");
        return -1;
    }

    AVPicture pict;
    //ros: int size = avpicture_fill(&pict, gBitmapRefPixelBuffer, AV_PIX_FMT_RGB565LE, gAbi.width, gAbi.height);
    int size = avpicture_fill(&pict, gBitmapRefPixelBuffer, AV_PIX_FMT_RGB565LE, gAbi.width, gAbi.height);
    if (size != gAbi.stride * gAbi.height)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "size != gAbi.stride * gAbi.height");
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "size = %d", size);
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "gAbi.stride * gAbi.height = %d", gAbi.stride * gAbi.height);
        return -2;
    }

    int height = sws_scale(gSwsContext, (const uint8_t* const*)gVideoFrame->data, gVideoFrame->linesize, 0, gVideoCodecCtx->height, pict.data, pict.linesize);
    if (height != gAbi.height)
    {
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "height != gAbi.height");
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "height = %d", height);
        __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "gAbi.height = %d", gAbi.height);
        return -3;
    }

    return 0;
}

JNIEXPORT jdouble JNICALL Java_com_jazzros_ffmpegtest_AVThread_nativeGetFps(JNIEnv* env, jobject thiz)
{
	AVStream *st = gFormatCtx->streams[gVideoStreamIdx];
	return (double)st->avg_frame_rate.num / (double)st->avg_frame_rate.den;
}
