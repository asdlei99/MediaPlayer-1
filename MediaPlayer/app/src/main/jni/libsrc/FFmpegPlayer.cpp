#include "FFmpegPlayer.hpp"
#include "FFmpegParameters.hpp"
#include "streams/FFmpegAudioStream.hpp"

#include "threads/ScopedLock.h"

#include <memory>

size_t getMemorySize();

namespace JAZZROS {

FFmpegPlayer::FFmpegPlayer() :
    m_commands(0)
{
//ros:    setOrigin(osg::Image::TOP_LEFT);

    std::auto_ptr<CommandQueue> commands(new CommandQueue); // todo: why here need auto_ptr<>?

    m_commands = commands.release();
}



FFmpegPlayer::FFmpegPlayer(const FFmpegPlayer & image) :
    ImageStream(image)
{
    // todo: probably incorrect or incomplete. Maybe it will be better to hide copy constructor?
}



FFmpegPlayer::~FFmpegPlayer()
{
    av_log(NULL, AV_LOG_INFO, "Destructing FFmpegPlayer...");

    quit(true);

    av_log(NULL, AV_LOG_INFO, "Have done quit");

    // release athe audio streams to make sure that the decoder doesn't retain any external
    // refences.
    getAudioStreams().clear(); // todo: I guess these objects should be deleted before clear()

    delete m_commands;

    av_log(NULL, AV_LOG_INFO, "Destructed FFmpegPlayer");
}

bool FFmpegPlayer::open(const std::string & filename, FFmpegParameters* parameters)
{
    av_log(NULL, AV_LOG_INFO, "FFmpeg plugin release version: %d", JAZZROS_FFMPEG_LIBRARY_RELEASE_VERSION_INT);
    av_log(NULL, AV_LOG_INFO, "OS physical RAM size: %d MB", getMemorySize() / 1000000);

    setFileName(filename);

    if (m_fileHolder.open(filename, parameters) < 0)
        return false;

    if (m_streamer.open(& m_fileHolder, this) < 0)
    {
        m_fileHolder.close();
        return false;
    }
    // If video exist...
    if (m_fileHolder.videoIndex() >= 0)
    {
        GLint                   internalTexFmt;
        GLint                   pixFmt;
        FFmpegFileHolder::getGLPixFormats (m_fileHolder.getPixFormat(), internalTexFmt, pixFmt);
/*
        setImage(
            m_fileHolder.width(), m_fileHolder.height(), 1, internalTexFmt, pixFmt, GL_UNSIGNED_BYTE,
            const_cast<unsigned char *>(m_streamer.getFrame()), NO_DELETE
        );
*/
        setImage(
                m_fileHolder.width(), m_fileHolder.height(), 1, internalTexFmt, pixFmt,
                const_cast<unsigned char *>(m_streamer.getFrame())
        );

        setPixelAspectRatio(m_fileHolder.pixelAspectRatio());

        av_log(NULL, AV_LOG_INFO, "File( %s ) size(%d, %d) aspect ratio %f",
               filename.c_str(),
               s(),t(),
               m_fileHolder.pixelAspectRatio());

        // swscale is reported errors and then crashing when rescaling video of size less than 10 by 10.
        if (s()<=10 || t()<=10)
            return false;
    }
    // If audio exist...
    if (m_fileHolder.isHasAudio())
    {
        av_log(NULL, AV_LOG_INFO, "Attaching FFmpegAudioStream");

        getAudioStreams().push_back(new FFmpegAudioStream(& m_fileHolder, & m_streamer));
    }

    _status = PAUSED;
    applyLoopingMode();

    start(); // start thread

    return true;
}

void FFmpegPlayer::close()
{
    m_streamer.close();
    m_fileHolder.close();
}

void FFmpegPlayer::play()
{
    pushCommand(CMD_PLAY);

#if 0
    // Wait for at least one frame to be published before returning the call
    TScopedLock<Mutex> lock(m_mutex);

    while (duration() > 0 && ! m_frame_published_flag)
        m_frame_published_cond.wait(&m_mutex);

#endif
}



void FFmpegPlayer::pause()
{
    pushCommand(CMD_PAUSE);
}



void FFmpegPlayer::rewind()
{
    pushCommand(CMD_REWIND);
}

void FFmpegPlayer::seek(double time)
{
    m_seek_time = time;
    pushCommand(CMD_SEEK);
}



void FFmpegPlayer::quit(bool waitForThreadToExit)
{
    // Stop control thread
    if (isRunning())
    {
        pushCommand(CMD_STOP);

        if (waitForThreadToExit)
            join();
    }
}

void FFmpegPlayer::setVolume(float volume)
{
    m_streamer.setAudioVolume(volume);
}

float FFmpegPlayer::getVolume() const
{
    return m_streamer.getAudioVolume();
}

float FFmpegPlayer::getAudioBalance()
{
    return m_streamer.getAudioBalance();
}

void FFmpegPlayer::setAudioBalance(float balance)
{
    m_streamer.setAudioBalance(balance);
}

double FFmpegPlayer::getCreationTime() const
{
    // V/A readers operate with time-values, based on ZERO time-point.
    // So here we returns 0.
    return 0;
}

double FFmpegPlayer::getLength() const
{
    return m_fileHolder.duration_ms();
}


double FFmpegPlayer::getReferenceTime () const
{
    return m_streamer.getCurrentTimeSec();
}

double FFmpegPlayer::getCurrentTime() const
{
    return m_streamer.getCurrentTimeSec();
}



double FFmpegPlayer::getFrameRate() const
{
    return m_fileHolder.frameRate();
}



bool FFmpegPlayer::isImageTranslucent() const
{
    return m_fileHolder.alphaChannel();
}



void FFmpegPlayer::run()
{
    av_log(NULL, AV_LOG_INFO, "Start FFmpegPlayer::run()");
    try
    {
        Mutex       lockMutex;
        ScopedLock  lock(lockMutex);

        bool done = false;

        while (! done)
        {
            if (_status == PLAYING)
            {
                bool no_cmd;
                const Command cmd = m_commands->timedPop(no_cmd, 1);

                if (no_cmd)
                {
                    m_commandQueue_cond.wait(& lockMutex);
                }
                else
                {
                    done = ! handleCommand(cmd);
                }
            }
            else
            {
                done = ! handleCommand(m_commands->pop());
            }
        }
    }

    catch (const std::exception & error)
    {
        //OSG_WARN << "FFmpegPlayer::run : " << error.what() << std::endl;
        av_log(NULL, AV_LOG_WARNING, "FFmpegPlayer::run : %s", error.what());
    }

    catch (...)
    {
        //OSG_WARN << "FFmpegPlayer::run : unhandled exception" << std::endl;
        av_log(NULL, AV_LOG_WARNING, "FFmpegPlayer::run : unhandled exception");
    }

    //OSG_NOTICE<<"Finished FFmpegPlayer::run()"<<std::endl;
    av_log(NULL, AV_LOG_INFO, "Finished FFmpegPlayer::run()");

    // Ensure that all threads are closed
    cmdPause();
    //
    // Because \open() starts thread, here we should call \close()
    close();
}



void FFmpegPlayer::applyLoopingMode()
{
    m_streamer.loop (getLoopingMode() == LOOPING);
}

void FFmpegPlayer::pushCommand(Command cmd)
{
    m_commands->push(cmd);
    m_commandQueue_cond.signal();
}

bool FFmpegPlayer::handleCommand(const Command cmd)
{
    switch (cmd)
    {
    case CMD_PLAY:
        cmdPlay();
        return true;

    case CMD_PAUSE:
        cmdPause();
        return true;

    case CMD_REWIND:
        cmdRewind();
        return true;

    case CMD_SEEK:
        cmdSeek(m_seek_time);
        return true;

    case CMD_STOP:
        cmdPause();
        return false;

    default:
        //OSG_WARN << "FFmpegPlayer::handleCommand() Unsupported command" << std::endl;
        av_log(NULL, AV_LOG_WARNING, "FFmpegPlayer::handleCommand() Unsupported command");
        return false;
    }
}



void FFmpegPlayer::cmdPlay()
{
    if (_status == PAUSED)
        m_streamer.play();

    _status = PLAYING;
}



void FFmpegPlayer::cmdPause()
{
    if (_status == PLAYING)
    {
        m_streamer.pause();
    }

    _status = PAUSED;
}



void FFmpegPlayer::cmdRewind()
{
    bool isPlayed = false;

    if (_status == PLAYING)
    {
        isPlayed = true;
        m_streamer.pause();
    }

    // V/A readers operate with time-values, based on ZERO time-point.
    // So here we seeking to ZERO-time point
    m_streamer.seek(0);

    if (isPlayed == true)
        m_streamer.play();
}

void FFmpegPlayer::cmdSeek(double time)
{
    cmdPause();
    //
    const unsigned long ul_time = time;
    m_streamer.seek(ul_time);
}

void FFmpegPlayer::setImage(const unsigned short &width, const unsigned short &height,
                      const int &someInt,
                      const GLint &interanlTexFormat,
                      const GLint &pixFormat,
                      unsigned char *pFramePtr)
{
    av_log(NULL, AV_LOG_INFO, "setImage(%d,%d)", // todo: that's all?
            width, height);
}


const size_t FFmpegPlayer::s() const
{
    return 100; // todo: really?
}

const size_t FFmpegPlayer::t() const
{
    return 100; // todo: really?
}

} // namespace JAZZROS
