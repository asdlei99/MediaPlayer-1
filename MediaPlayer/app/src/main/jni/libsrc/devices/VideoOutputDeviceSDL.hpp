#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H

#include "VideoOutputDevice.hpp"
#include <SDL.h>

void my_function(void *pParam);

namespace JAZZROS {

    class VideoOutputDeviceSDL : public VideoOutputDevice {

    friend void my_function(void *pParam);

        SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *bitmapTex;

    private:

        virtual const int       Initialize (VideoOutputDeviceData *, const FFmpegFileHolder & );

        virtual const int       render(const unsigned char * pFrame) const;

    public:

        VideoOutputDeviceSDL();
        ~VideoOutputDeviceSDL();

    }; // class VideoOutputDeviceSDL

} // namespace JAZZROS

#endif // HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H
