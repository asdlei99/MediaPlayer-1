#ifndef HEADER_GUARD_IMAGESTREAM_H
#define HEADER_GUARD_IMAGESTREAM_H

#include <list>
#include <string>
#include "AudioStream.hpp"

//#define GLint   int
//#include <GL/gl.h>
#include <GLES2/gl2.h>

namespace JAZZROS {


    class ImageStream {
        std::list <AudioStream*> m_audio_streams;
        std::string m_filename;
        float m_pixAspectRatio;


    protected:
        enum PlayingStatus {
            PLAYING,
            PAUSED
        };
        PlayingStatus   _status;
        enum LoopingMode {
            SINGLE,
            LOOPING
        };
        LoopingMode m_loopMode;
    public:
        void setFileName(const std::string &filename) {
            m_filename = filename;
        }

        void setPixelAspectRatio(const float & value) {
            m_pixAspectRatio = value;
        }

        virtual void setImage(const unsigned short &width, const unsigned short &height,
                              const int &someInt,
                              const GLint &interanlTexFormat,
                              const GLint &pixFormat,
                              unsigned char *pFramePtr) = 0;

        std::list <AudioStream*> &getAudioStreams() {
            return m_audio_streams;
        }

        virtual const size_t s() const = 0;

        virtual const size_t t() const = 0;

        const LoopingMode getLoopingMode() const {
            return m_loopMode;
        }
    };
} // namespace JAZZROS

#endif // HEADER_GUARD_IMAGESTREAM_H