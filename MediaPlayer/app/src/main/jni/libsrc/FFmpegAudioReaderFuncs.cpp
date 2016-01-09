#include "libsrc/readers/FFmpegAudioReader.hpp"



JAZZROS::FFmpegAudioReader*  gAudioReader;


extern "C"
{

#ifdef USE_AV_LOCK_MANAGER
    #include "libsrc/ThreadLock.h"
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


} // extern "C"