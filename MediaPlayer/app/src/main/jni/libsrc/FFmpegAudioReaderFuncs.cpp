#include "readers/FFmpegAudioReader.hpp"



JAZZROS::FFmpegAudioReader*  gAudioReader;

#include "FFmpegParameters.hpp"
#include "FFmpegPlayer.hpp"
#include "devices/VideoOutputDeviceSDL.hpp"
#include "devices/VideoOutputDeviceGL.hpp"
#include "AudioSinkManager.h"


//JAZZROS::FFmpegPlayer           gPlayer;
JAZZROS::FFmpegPlayer           gPlayerArray[2];
JAZZROS::VideoOutputDevice *    gOutputDevicePtr = NULL;
std::size_t                     gPlayerArrayNb = 0;


extern "C"
{

#ifdef USE_AV_LOCK_MANAGER
    #include "threads/ThreadLock.h"
    static int lockMgr(void **mutex, enum AVLockOp op)
    {
        // returns are 0 success
        struct CThreadLock **m=(struct CThreadLock**)mutex;
        if (op==AV_LOCK_CREATE)
        {
            *m = new CThreadLock();
            return !*m;
        }
        else if (op==AV_LOCK_DESTROY)
        {
            delete *m;
            return 0;
        }
        else if (op==AV_LOCK_OBTAIN)
        {
            (*m)->Lock();
            return 0;
        }
        else if (op==AV_LOCK_RELEASE)
        {
            (*m)->Unlock();
            return 0;
        }
        else
        {
            return -1;
        }
    }

    /* "global" pointer */
    int (*pfLockMgr)(void **mutex, enum AVLockOp op);

    void initializePfLockMgr(void)
    {
        pfLockMgr = lockMgr;
    }
#endif


const int gAudioReaderInit()
{
    gAudioReader = NULL;
    return 0;
}

void gAudioReaderClose()
{
    if (gAudioReader) {
        gAudioReader->close();
        delete gAudioReader;
        gAudioReader = NULL;
    }
}

const int gAudioReaderOpen(const char * mfileName)
{
    if (gAudioReader)
        gAudioReaderClose();

    gAudioReader = new JAZZROS::FFmpegAudioReader();

    if (gAudioReader->openFile(mfileName, NULL) < 0) {
        // __android_log_print(ANDROID_LOG_ERROR, "com.jazzros.ffmpegtest", "FFmpegAudioReader cannot open file");
        delete gAudioReader;
        gAudioReader = NULL;
        return -1;
    }
    return 0;
}


const int gAudioReaderGetSamples(unsigned long * msTime,
                                 unsigned short channelsNb,
                                 const AVSampleFormat * output_sampleFormat,
                                 unsigned short * sample_rate,
                                 unsigned long * samplesNb,
                                 unsigned char * bufSamples,
                                 const double * max_avail_time_micros)
{
    if (gAudioReader)
    {
        return JAZZROS::FFmpegAudioReader::getSamples(gAudioReader,
                                                     * msTime,
                                                     channelsNb,
                                                     * output_sampleFormat,
                                                     * sample_rate,
                                                     * samplesNb,
                                                     bufSamples,
                                                     * max_avail_time_micros);
    }
    return -1;
}

const int   gPlayerInit()
{
    if (gOutputDevicePtr == NULL) {
        gOutputDevicePtr = new JAZZROS::VideoOutputDeviceSDL();
//        gOutputDevicePtr = new JAZZROS::VideoOutputDeviceGL();
    }
    if (AudioSinkManager::initialize() != 0)
        return -1;

    gPlayerArrayNb = 0;

    return 0;
}
const int   gPlayerRelease()
{
    if (gOutputDevicePtr) {
        delete gOutputDevicePtr;
        gOutputDevicePtr = NULL;
    }
    if (AudioSinkManager::release() != 0)
        return -1;

    return 0;
}

const int   gPlayerOpen(const char * pFileName)
{
    if (gOutputDevicePtr == NULL)
    {
        return -1;
    }


    JAZZROS::FFmpegParameters * parameters = new JAZZROS::FFmpegParameters();
///    av_dict_set(parameters->getOptions(), "video_size", "1280:720", 0);
//    av_dict_set(parameters->getOptions(), "video_size", "2048:2048", 0);
    //
    JAZZROS::FFmpegPlayer & gPlayer = gPlayerArray[gPlayerArrayNb];

    if (gPlayer.open(pFileName, parameters, gOutputDevicePtr) == true) {

        /**
         * After player's method open() has been through successfully, it's object contains
         * audio streams list. Audio stream should be initialized of real device for output.
         */
        gPlayer.activateOutput();

        gPlayerArrayNb++;

        return gPlayerArrayNb - 1;
    }
    return -1;
}

const int   gPlayerPlay(const int & index)
{
    gPlayerArray[index].play();
    return 0;
}
const int   gPlayerPause(const int & index)
{
    gPlayerArray[index].pause();
    return 0;
}
const int   gPlayerSelectCurrent(const int & index)
{
    JAZZROS::FFmpegPlayer & player = gPlayerArray[index];

    player.activateOutput();

    return 0;
}
const int   gPlayerSeek(const int & index, const double & percent)
{
    const double length = gPlayerArray[index].getLength();
    gPlayerArray[index].seek(length * percent / 100.0);

    return 0;
}
void        gPlayerQuit(const int & index)
{
    gPlayerArray[index].quit();
}
const int   gGetPlayerStatus(const int & index)
{
    switch (gPlayerArray[index].get_status()) {
        case JAZZROS::ImageStream::PAUSED:
            return 0;
        case JAZZROS::ImageStream::PLAYING:
            return 1;
    };
    return -1;
}

} // extern "C"
