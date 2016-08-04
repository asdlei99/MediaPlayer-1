#ifndef HEADER_GUARD_FFMPEG_RENDERTHREAD_H
#define HEADER_GUARD_FFMPEG_RENDERTHREAD_H

#include <OpenThreads/Thread>
#include "../FFmpegFileHolder.hpp"
#include "../devices/VideoOutputDevice.hpp"

namespace JAZZROS {

class FFmpegILibAvStreamImpl;
class FFmpegFileHolder;





class FFmpegRenderThread : protected OpenThreads::Thread
{
    VideoOutputDevice           * m_pOutputDevice;
    VideoOutputDeviceData       * m_pOutputDeviceData;
    FFmpegILibAvStreamImpl      * m_pLibAvStream;
    const FFmpegFileHolder      * m_pFileHolder;
    volatile bool               m_renderingThreadStop;

    virtual void                run();
public:

    virtual                     ~FFmpegRenderThread();

    const int                   Initialize(FFmpegILibAvStreamImpl *, VideoOutputDevice *, VideoOutputDeviceData *, const FFmpegFileHolder * pFileHolder);

    void                        Start();
    void                        Stop();
    virtual void                quit(bool waitForThreadToExit = true);
};

} // namespace JAZZROS

#endif // HEADER_GUARD_FFMPEG_RENDERTHREAD_H
