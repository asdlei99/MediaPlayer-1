#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICE_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICE_H

#include "VideoOutputDeviceData.hpp"

namespace JAZZROS {

    class VideoOutputDevice {

    private:

        VideoOutputDeviceData * m_vodd;

    public:

                                VideoOutputDevice();
        virtual                 ~VideoOutputDevice();

        void                    SetCurrentData(VideoOutputDeviceData * ptr);

        virtual const int       Initialize (void) = 0;

        VideoOutputDeviceData * getData() const;
        virtual VideoOutputDeviceData * CreateData() const = 0;

        virtual const int       render(VideoOutputDeviceData * sender, const unsigned char * pFrame) const = 0;

    }; // class VideoOutputDevice

} // namespace JAZZROS

#endif // HEADER_GUARD_VIDEOOUTPUTDEVICE_H
