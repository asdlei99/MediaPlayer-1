#ifndef HEADER_GUARD_IMAGESTREAM_H
#define HEADER_GUARD_IMAGESTREAM_H

#include <vector>
#include <string>
#include "AudioStream.hpp"

#ifdef ANDROID
    #include <GLES2/gl2.h>
#else // ANDROID
    #include <GL/gl.h>
#endif // ANDROID

namespace JAZZROS {


    class ImageStream {
    public:
        enum PlayingStatus {
            PLAYING,
            PAUSED
        };
    private:
        std::vector<AudioStream *> m_audio_streams;
        std::string m_filename;
        float m_pixAspectRatio;


    protected:
        enum LoopingMode {
            SINGLE,
            LOOPING
        };
        LoopingMode m_loopMode;
        PlayingStatus _status;
    public:
        ImageStream() : m_loopMode(SINGLE), _status(PAUSED) {
        }

        void setFileName(const std::string &filename) {
            m_filename = filename;
        }
        const std::string & getFileName(void) const {
            return m_filename;
        }

        void setPixelAspectRatio(const float &value) {
            m_pixAspectRatio = value;
        }

        virtual void setImage(const unsigned short &width, const unsigned short &height,
                              const int &someInt,
                              const GLint &interanlTexFormat,
                              const GLint &pixFormat,
                              const unsigned char *pFramePtr) const = 0;

        std::vector<AudioStream *> &getAudioStreams() {
            return m_audio_streams;
        }

        virtual const size_t s() const = 0;

        virtual const size_t t() const = 0;

        const LoopingMode getLoopingMode() const {
            return m_loopMode;
        }

        const PlayingStatus &get_status() const {
            return _status;
        }
    };
} // namespace JAZZROS

#endif // HEADER_GUARD_IMAGESTREAM_H
