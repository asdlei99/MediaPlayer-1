#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICE_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICE_H

#include "VideoOutputDeviceData.hpp"

namespace JAZZROS {

    class VideoOutputDevice {

    private:

        VideoOutputDeviceData * m_vodd;

        virtual const int       Initialize (VideoOutputDeviceData *, const FFmpegFileHolder & ) = 0;

    public:

                                VideoOutputDevice(VideoOutputDeviceData *);
        virtual                 ~VideoOutputDevice();

        const int               Initialize(const FFmpegFileHolder & );

        const VideoOutputDeviceData * getData() const;

        virtual const int       render(const unsigned char * pFrame) = 0;

    }; // class VideoOutputDevice

} // namespace JAZZROS

#endif // HEADER_GUARD_VIDEOOUTPUTDEVICE_H
