#include "readers/FFmpegAudioReader.hpp"



JAZZROS::FFmpegAudioReader*  gAudioReader;

#include "FFmpegParameters.hpp"
#include "FFmpegPlayer.hpp"
#include "devices/VideoOutputDeviceSDL.hpp"
#include "devices/VideoOutputDeviceGL.hpp"
#include "AudioSinkManager.h"
#include <map>


//JAZZROS::FFmpegPlayer           gPlayer;
//JAZZROS::FFmpegPlayer           gPlayerArray[2];
//std::vector<JAZZROS::FFmpegPlayer*> gPlayerArray;
std::map<int, JAZZROS::FFmpegPlayer* > gPlayerArray;
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
    if (gPlayerArray.find(gPlayerArrayNb) == gPlayerArray.end())
        gPlayerArray.insert(std::make_pair(gPlayerArrayNb, new JAZZROS::FFmpegPlayer()));

    JAZZROS::FFmpegPlayer * gPlayer = gPlayerArray[gPlayerArrayNb];

    if (gPlayer->open(pFileName, parameters, gOutputDevicePtr) == true) {

        /**
         * After player's method open() has been through successfully, it's object contains
         * audio streams list. Audio stream should be initialized of real device for output.
         */
        gPlayer->activateOutput();

        gPlayerArrayNb++;

        return gPlayerArrayNb - 1;
    }
    return -1;
}

const int   gPlayerPlay(const int & index)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {
        player->play();
        return 0;
    }
    return -1;
}
const int   gPlayerPause(const int & index)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {
        player->pause();
        return 0;
    }
    return -1;
}

const int   gPlayerSeek(const int & index, const double & percent)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {

        const double length = player->getLength();
        player->seek(length * percent / 100.0);

        return 0;
    }
    return -1;
}
const int   gPlayerSeekMS(const int & index, const int & ms)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {
        player->seek(ms);
        return 0;
    }

    return -1;
}
void        gPlayerQuit(const int & index)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {
        player->quit();
        /**
            We cannot delete objects of player
            because it could be used in other objects.
            For example - last Player in AudioSinkManager;
        delete player;
        gPlayerArray.erase(index);
        */
    }
}
const int   gGetPlayerStatus(const int & index)
{
    JAZZROS::FFmpegPlayer * player = gPlayerArray[index];

    if (player) {
        switch (player->get_status()) {
            case JAZZROS::ImageStream::PAUSED:
                return 0;
            case JAZZROS::ImageStream::PLAYING:
                return 1;
        };
    }

    return -1;
}
const int   gPlayerRelease()
{
    int i;

    for (i = 0; i < gPlayerArrayNb; ++i)
        gPlayerQuit (i);

    if (gOutputDevicePtr) {
        delete gOutputDevicePtr;
        gOutputDevicePtr = NULL;
    }
    if (AudioSinkManager::release() != 0)
        return -1;

    for (i = 0; i < gPlayerArrayNb; ++i)
    {
        JAZZROS::FFmpegPlayer * player = gPlayerArray[i];
        if (player) {
            delete player;
            gPlayerArray.erase(i);
        }
    }

    return 0;
}

} // extern "C"
