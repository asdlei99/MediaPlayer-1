#include "readers/FFmpegAudioReader.hpp"



JAZZROS::FFmpegAudioReader*  gAudioReader;

#include "FFmpegParameters.hpp"
#include "FFmpegPlayer.hpp"
#include "devices/VideoOutputDeviceSDL.hpp"
#include "devices/VideoOutputDeviceGL.hpp"


#include "SDLAudioSink.h"

JAZZROS::FFmpegPlayer           gPlayer;
JAZZROS::VideoOutputDeviceSDL   gOutputDevice;

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

const int   gPlayerOpen(const char * pFileName)
{


    JAZZROS::FFmpegParameters * parameters = new JAZZROS::FFmpegParameters();
///    av_dict_set(parameters->getOptions(), "video_size", "1280:720", 0);
//    av_dict_set(parameters->getOptions(), "video_size", "2048:2048", 0);
    //
    if (gPlayer.open(pFileName, parameters, & gOutputDevice) == true) {

        /**
         * After player's method open() has been through successfully, it's object contains
         * audio streams list. Audio stream should be initialized of real device for output.
         */
        std::vector<JAZZROS::AudioStream *> & audioStreamList = gPlayer.getAudioStreams();

        if (audioStreamList.size() > 0) {

            JAZZROS::AudioStream *   audioStream = audioStreamList.front();
            audioStream->setAudioSink (new SDLAudioSink(audioStream));
        }

        return 0;
    }
    return -1;
}

const int   gPlayerPlay()
{
    gPlayer.play();
    return 0;
}
const int   gPlayerPause()
{
    gPlayer.pause();
    return 0;
}
const int   gGetPlayerStatus()
{
    switch (gPlayer.get_status()) {
        case JAZZROS::ImageStream::PAUSED:
            return 0;
        case JAZZROS::ImageStream::PLAYING:
            return 1;
    };
    return -1;
}

} // extern "C"