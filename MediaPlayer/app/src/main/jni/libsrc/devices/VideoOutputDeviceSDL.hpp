#ifndef HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H
#define HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H

#include "VideoOutputDevice.hpp"
#include <SDL.h>

void my_function(void *pParam);
enum SDL_ENUM_CODE
{
    SDL_USEREVENT_CODE_RENDER = 0,
    SDL_USEREVENT_CODE_TIMER1 = 1,
    SDL_USEREVENT_CODE_PLAYERFINISHED = 2
};

namespace JAZZROS {

    class VideoOutputDeviceSDL : public VideoOutputDevice {

    struct sParam {
        const VideoOutputDeviceSDL * pThis;
        const VideoOutputDeviceData * pSender;
        const unsigned char * pFramePtr;
    };
    mutable sParam  m_render_param;

    friend void my_function(void *pParam);

        SDL_Window *window;
        SDL_Renderer *renderer;

    private:

        virtual VideoOutputDeviceData * CreateData() const;

        virtual const int       Initialize (void);

        virtual const int       render(VideoOutputDeviceData * sender, const unsigned char * pFrame) const;

    public:

        VideoOutputDeviceSDL();
        ~VideoOutputDeviceSDL();

        SDL_Renderer * getRenderer() {return renderer;}

    }; // class VideoOutputDeviceSDL

} // namespace JAZZROS

#endif // HEADER_GUARD_VIDEOOUTPUTDEVICESDL_H
