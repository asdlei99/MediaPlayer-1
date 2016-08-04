#include "VideoOutputDeviceGL.hpp"
//#include "VideoOutputDeviceData.hpp"
#include "ImageStream.hpp" // for GLint, and setImage

namespace JAZZROS {

    class VideoOutputDeviceDataGL : public VideoOutputDeviceData {
    private:

        virtual const int Initialize(VideoOutputDevice * pVOD, const Size & frameSize) {

            /**
             * Initialize parent class' object
             */
            VideoOutputDeviceData::Initialize(pVOD, frameSize);

            /**
             * Initialize local class object
             */
            switch (getFFPixelFormat()) {

                case AV_PIX_FMT_RGB24: {
                    outInternalTexFmt = GL_RGB;
                    outPixFmt = GL_RGB;
                    break;
                }
                case AV_PIX_FMT_BGR24: {
                    outInternalTexFmt = GL_RGB;
#ifdef ANDROID
                    outPixFmt = GL_RGB;
#else
                    outPixFmt = GL_RGB;
#endif
                    break;
                }
                case AV_PIX_FMT_BGRA: {
                    outInternalTexFmt = GL_RGBA;
#ifdef ANDROID
                    outPixFmt = GL_RGBA;
#else
                    outPixFmt = GL_RGBA;
#endif
                    break;
                }
                case AV_PIX_FMT_RGBA: {
                    outInternalTexFmt = GL_RGBA;
                    outPixFmt = GL_RGBA;
                    break;
                }
                default: {
                    av_log(NULL, AV_LOG_WARNING, "Cannot find GL pix format for libav pix format");
                    return -1;
                }
            };
            return 0;
        }
    public:
        GLint outInternalTexFmt;
        GLint outPixFmt;

        VideoOutputDeviceDataGL()
                :VideoOutputDeviceData(AV_PIX_FMT_BGR24),
                 outInternalTexFmt(GL_RGB),
                 outPixFmt(GL_RGB)
        {}
    };

    VideoOutputDeviceGL::VideoOutputDeviceGL()
    : m_pImgStream(NULL){
    }

    VideoOutputDeviceData * VideoOutputDeviceGL::CreateData() const
    {
        return new VideoOutputDeviceDataGL();
    }

    const int
    VideoOutputDeviceGL::Initialize (void) {

        // local initialization

        return 0;
    }

    void VideoOutputDeviceGL::setImageStream(ImageStream * ptr) {
        m_pImgStream = ptr;
    }

    const int
    VideoOutputDeviceGL::render(VideoOutputDeviceData * sender, const unsigned char * pFramePtr) const {

        const VideoOutputDeviceDataGL * pData     = dynamic_cast<const VideoOutputDeviceDataGL*>(getData());

        if (sender != pData)
            return 0;

        if (m_pImgStream && pData) {

            const Size      frameSize   = pData->getFrameSize();

            m_pImgStream->setImage(
                    frameSize.Width,
                    frameSize.Height,
                    1, pData->outInternalTexFmt, pData->outPixFmt,
                    pFramePtr);
            return 0;
        }
        return -1;
    }

} // namespace JAZZROS
