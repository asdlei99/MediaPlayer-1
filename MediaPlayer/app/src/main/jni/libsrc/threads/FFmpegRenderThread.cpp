#include "FFmpegRenderThread.hpp"
#include "../streams/FFmpegILibAvStreamImpl.hpp"
#include "../FFmpegFileHolder.hpp"
#include "../system/Timer.h"

namespace JAZZROS {


FFmpegRenderThread::~FFmpegRenderThread()
{
    quit(true);
}

const int
FFmpegRenderThread::Initialize(FFmpegILibAvStreamImpl * pSrc, ImageStream * pDst, const FFmpegFileHolder * pFileHolder)
{
    m_pFileHolder   = pFileHolder;
    m_pLibAvStream  = pSrc;
    m_pImgStream    = pDst;

    if (pSrc == NULL || pDst == NULL)
        return -1;

    return 0;
}

void
FFmpegRenderThread::Start()
{
    //
    // Guaranty that thread will starts even if it was run before
    //
    if (isRunning() == true)
        Stop();

    if (isRunning() == false)
    {
        // To avoid thread concurent conflicts, follow param should be
        // defined from parent thread
        m_renderingThreadStop = false;

        // start thread
        start();
    }
}

void
FFmpegRenderThread::run()
{
    try
    {
        unsigned char *         pFramePtr;
        unsigned long           timePosMS;

        Timer                   loopTimer;
        loopTimer.setStartTick(0);
        const float             frameTimeMS = 1000.0f / m_pLibAvStream->fps();
        double                  tick_start_ms = -1.0;
        double                  tick_end_ms;
        double                  frame_ms;
        double                  dist_frame_ms;
        int                     iErr;
        //
        GLint                   internalTexFmt;
        GLint                   pixFmt;
        FFmpegFileHolder::getGLPixFormats (m_pFileHolder->getPixFormat(), internalTexFmt, pixFmt);
        //
        while (m_renderingThreadStop == false)
        {
            //
            //
            timePosMS = m_pLibAvStream->GetPlaybackTime();

            iErr = m_pLibAvStream->GetFramePtr (timePosMS, pFramePtr);
            //
            // Frame could be not best time position(iErr > 0),
            // but to avoid stucking, we should draw it
            if (pFramePtr != NULL && iErr >= 0)
            {
                //
                // Calculate extra time to sleep thread before next the frame is appear
                //
                if (tick_start_ms >= 0.0)
                {
                    tick_end_ms = loopTimer.time_m();
                    frame_ms = tick_end_ms - tick_start_ms;
                    dist_frame_ms = frameTimeMS - frame_ms;
                    if (dist_frame_ms > 0.0 && dist_frame_ms < frameTimeMS)
                    {
                        //
                        // Devide by 2 for avoid twitching of the image which smoothly changes the background
                        //
//ros:                        OpenThreads::Thread::microSleep(1000 * dist_frame_ms / 2);
                        Thread::microSleep(1000 * dist_frame_ms / 2);
                    }
                }
                /*
ros:
                    m_pImgStream->setImage(
                    m_pFileHolder->width(),
                    m_pFileHolder->height(),
                    1, internalTexFmt, pixFmt, GL_UNSIGNED_BYTE,
                    pFramePtr, osg::Image::NO_DELETE
                );
                */
                m_pImgStream->setImage(
                        m_pFileHolder->width(),
                        m_pFileHolder->height(),
                        1, internalTexFmt, pixFmt,
                        pFramePtr
                );

                tick_start_ms = loopTimer.time_m();
            }

            m_pLibAvStream->ReleaseFoundFrame();
        }
    }
    catch (const std::exception & error)
    {
        //OSG_WARN << "FFmpegRenderThread::run : " << error.what() << std::endl;
    }

    catch (...)
    {
        //OSG_WARN << "FFmpegRenderThread::run : unhandled exception" << std::endl;
    }
}

void
FFmpegRenderThread::quit(bool waitForThreadToExit)
{
    m_renderingThreadStop = true;
    if (isRunning())
    {
        if (waitForThreadToExit)
            join();
    }
}

void
FFmpegRenderThread::Stop()
{
    quit (true);
}

} // namespace JAZZROS
