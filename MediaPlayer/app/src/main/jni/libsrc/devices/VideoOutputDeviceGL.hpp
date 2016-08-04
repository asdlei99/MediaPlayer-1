#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICEGL_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICEGL_H

#include "VideoOutputDevice.hpp"

namespace JAZZROS {

    class ImageStream;

    class VideoOutputDeviceGL : public VideoOutputDevice {

    ImageStream                 * m_pImgStream;
    private:

        virtual VideoOutputDeviceData * CreateData() const;

        virtual const int       Initialize (void);

        virtual const int       render(VideoOutputDeviceData * sender, const unsigned char * pFrame) const;

    public:

        VideoOutputDeviceGL();

        void setImageStream(ImageStream *);

    }; // class VideoOutputDeviceGL

} // namespace JAZZROS

#endif // HEADER_GUARD_VIDEOOUTPUTDEVICEGL_H
