#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICEDATA_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICEDATA_H

#include "../FFmpegHeaders.hpp"
#include "../FFmpegFileHolder.hpp"

namespace JAZZROS {

    class VideoOutputDeviceData {

    private:

        Size                    m_frameSize;
        AVPixelFormat           m_pixFmt;

    public:

        virtual const int Initialize(const FFmpegFileHolder & pFileHolder) {
            m_pixFmt = pFileHolder.getPixFormat2();
            m_frameSize = pFileHolder.getFrameSize();
            return 0;
        }

    public:

        VideoOutputDeviceData(const AVPixelFormat & pixFmt):m_pixFmt(pixFmt){}


        unsigned int    getMemoeryFrameSize (void) const {
            return avpicture_get_size (m_pixFmt, m_frameSize.Width, m_frameSize.Height);
        }

        const AVPixelFormat getFFPixelFormat() const {
            return m_pixFmt;
        }

        const Size getFrameSize() const {
            return m_frameSize;
        }

    }; // class VideoOutputDeviceData

} // namespace JAZZROS

#endif //HEADER_GUARD_VIDEOOUTPUTDEVICEDATA_H
