#include "VideoOutputDevice.hpp"

namespace JAZZROS {


    VideoOutputDevice::VideoOutputDevice()
            : m_vodd(NULL) {

    }

    VideoOutputDevice::~VideoOutputDevice() {
    }

    void
    VideoOutputDevice::SetCurrentData(VideoOutputDeviceData * ptr) {
        m_vodd = ptr;
    }

    VideoOutputDeviceData *
    VideoOutputDevice::getData() const {
        return m_vodd;
    }

} // namespace JAZZROS
