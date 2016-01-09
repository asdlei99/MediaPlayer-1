#ifndef HEADER_GUARD_FFMPEG_RENDERTHREAD_H
#define HEADER_GUARD_FFMPEG_RENDERTHREAD_H

//#include <osg/ImageStream>
#include <OpenThreads/Thread>
#include "../FFmpegFileHolder.hpp"

namespace JAZZROS {

class FFmpegILibAvStreamImpl;
class FFmpegFileHolder;


#define GLint   int
class ImageStream
{
public:
virtual void setImage(const unsigned short & width, const unsigned short & height, const int & someInt,
              const GLint & interanlTexFormat,
                const GLint & pixFormat,
                unsigned char * pFramePtr) = 0;
};


class FFmpegRenderThread : protected OpenThreads::Thread
{
    ImageStream                 * m_pImgStream;
    FFmpegILibAvStreamImpl      * m_pLibAvStream;
    const FFmpegFileHolder      * m_pFileHolder;
    volatile bool               m_renderingThreadStop;

    virtual void                run();
public:

    virtual                     ~FFmpegRenderThread();

    const int                   Initialize(FFmpegILibAvStreamImpl *, ImageStream *, const FFmpegFileHolder * pFileHolder);

    void                        Start();
    void                        Stop();
    virtual void                quit(bool waitForThreadToExit = true);
};

} // namespace JAZZROS

#endif // HEADER_GUARD_FFMPEG_RENDERTHREAD_H
