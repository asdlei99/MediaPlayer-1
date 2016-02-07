#include "VideoOutputDevice.hpp"

namespace JAZZROS {


    VideoOutputDevice::VideoOutputDevice(VideoOutputDeviceData *pObj)
            : m_vodd(pObj) {

    }

    VideoOutputDevice::~VideoOutputDevice() {
        if (m_vodd) {
            delete m_vodd;
            m_vodd = NULL;
        }
    }

    const int
    VideoOutputDevice::Initialize(const FFmpegFileHolder & pFileHolder) {
        return Initialize(m_vodd, pFileHolder);
    }

    const VideoOutputDeviceData *
    VideoOutputDevice::getData() const {
        return m_vodd;
    }

} // namespace JAZZROS