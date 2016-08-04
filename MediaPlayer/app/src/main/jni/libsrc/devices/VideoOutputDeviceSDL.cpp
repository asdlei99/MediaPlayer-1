#include "VideoOutputDeviceSDL.hpp"

#include "../threads/ScopedLock.h"
typedef OpenThreads::Mutex          Mutex;
typedef JAZZROS::TScopedLock<Mutex> ScopedLock;
Mutex                               g_lockMutex;

extern void (*draw_interrupter)(unsigned char *, int,
                                 unsigned char *, int,
                                 unsigned char *, int);

namespace JAZZROS {

    class VideoOutputDeviceDataSDL : public VideoOutputDeviceData {
    private:
        virtual const int Initialize(VideoOutputDevice * pVOD, const Size & frameSize) {

            /**
             * Initialize parent class' object
             */
            VideoOutputDeviceData::Initialize(pVOD, frameSize);

            /**
             * Initialize local class object
             */
            switch (getFFPixelFormat()) {

                case AV_PIX_FMT_RGB24: {
                    m_sdlPixelFormat = SDL_PIXELFORMAT_RGB24;
                    break;
                }
                case AV_PIX_FMT_BGR24: {
                    m_sdlPixelFormat = SDL_PIXELFORMAT_BGR24;
                    break;
                }
                case AV_PIX_FMT_RGB565LE:{
                    m_sdlPixelFormat = SDL_PIXELFORMAT_RGB565;
                    break;
                }
                case AV_PIX_FMT_YUV420P: {
                    m_sdlPixelFormat = SDL_PIXELFORMAT_YV12;
                    break;
                }
                default: {
                    av_log(NULL, AV_LOG_WARNING, "Cannot find SDL pix format for libav pix format");
                    return -1;
                }
            };

            VideoOutputDeviceSDL * pVOD_SDL = dynamic_cast<VideoOutputDeviceSDL *>(pVOD);
            if (pVOD_SDL == NULL) {
                av_log(NULL, AV_LOG_WARNING, "Not correct using of App architecture");
                return -1;
            }

            bitmapTex = SDL_CreateTexture(pVOD_SDL->getRenderer(),
                                          m_sdlPixelFormat,
                                          SDL_TEXTUREACCESS_STREAMING, // texture will be able modifying
                                          frameSize.Width,
                                          frameSize.Height);
            if (bitmapTex == NULL)
                return -1;

            return 0;
        }
    public:
        SDL_Texture *bitmapTex;
        int     m_sdlPixelFormat;
        VideoOutputDeviceDataSDL()
                :VideoOutputDeviceData(AV_PIX_FMT_YUV420P)
                //:VideoOutputDeviceData(AV_PIX_FMT_RGB565LE)
                ,bitmapTex(NULL)
        {}
        virtual ~VideoOutputDeviceDataSDL()
        {
            if (bitmapTex)
                SDL_DestroyTexture(bitmapTex);
        }
    };

    // https://forums.libsdl.org/viewtopic.php?t=9898
    unsigned char * gplane0; int gplane0s;
    unsigned char * gplane1; int gplane1s;
    unsigned char * gplane2; int gplane2s;
    void SDL_updateYUVTexture(unsigned char * plane0, int plane0s,
                              unsigned char * plane1, int plane1s,
                              unsigned char * plane2, int plane2s) {

        gplane0 = plane0;
        gplane1 = plane1;
        gplane2 = plane2;
        gplane0s = plane0s;
        gplane1s = plane1s;
        gplane2s = plane2s;
    }

    VideoOutputDeviceSDL::VideoOutputDeviceSDL()
            : window(NULL)
            ,renderer(NULL)
{

        /*
            Enable stream-mode.
            It does not solve sync-issue, but avoid of buffering at all.
        */
        // draw_interrupter = SDL_updateYUVTexture;
    }

    VideoOutputDeviceSDL::~VideoOutputDeviceSDL() {
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
    }

    VideoOutputDeviceData * VideoOutputDeviceSDL::CreateData() const
    {
        return new VideoOutputDeviceDataSDL();
    }

    const int
    VideoOutputDeviceSDL::Initialize (void) {

        if (window == NULL)
            window = SDL_CreateWindow("SDL Media Player Window",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      640, 480, SDL_WINDOW_SHOWN);
        if (window == NULL)
            return -1;

        if (renderer == NULL)
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
            return -1;

        return 0;
    }


    void my_function(void *pParam) {

        ScopedLock  lock(g_lockMutex);

        VideoOutputDeviceSDL::sParam *  param = reinterpret_cast<VideoOutputDeviceSDL::sParam *>(pParam);

        const VideoOutputDeviceSDL *    pThis = param->pThis;

        const VideoOutputDeviceData *    pSender = param->pSender;

        const VideoOutputDeviceDataSDL *    pData     = dynamic_cast<const VideoOutputDeviceDataSDL*>(pThis->getData());

        if (pSender != pData)
            return;

        if (draw_interrupter == NULL)
        {

            const unsigned char *               pFramePtr = (const unsigned char *)(param->pFramePtr);

            void *                              mPixels = NULL;
            int                                 mPitch = 0;
            unsigned int                        frame_size = pData->getMemoeryFrameSize();

            SDL_LockTexture(pData->bitmapTex, NULL, reinterpret_cast<void **>(&mPixels), &mPitch);
            SDL_memcpy(mPixels, pFramePtr, frame_size);
            SDL_UnlockTexture(pData->bitmapTex);
        }
        else
        {
            SDL_UpdateYUVTexture(pData->bitmapTex, NULL,
                                 gplane0, gplane0s,
                                 gplane1, gplane1s,
                                 gplane2, gplane2s);
        }

        SDL_RenderClear(pThis->renderer);
        SDL_RenderCopy(pThis->renderer, pData->bitmapTex, NULL, NULL);
        SDL_RenderPresent(pThis->renderer);
    }

    const int
    VideoOutputDeviceSDL::render(VideoOutputDeviceData * sender, const unsigned char * pFramePtr) const {

        const VideoOutputDeviceDataSDL * pData     = dynamic_cast<const VideoOutputDeviceDataSDL*>(getData());

        if (pData) {

            if (sender != pData)
                return 0;

            m_render_param.pThis = this;
            m_render_param.pSender = sender;
            m_render_param.pFramePtr = pFramePtr;
            /**
             *
             */
            SDL_Event       event;
            SDL_UserEvent   userevent;

            userevent.type = SDL_USEREVENT;
            userevent.code = 0;
            userevent.data1 = (void *)(& my_function);;
            userevent.data2 = (void *)(& m_render_param);

            event.type = SDL_USEREVENT;
            event.user = userevent;

            SDL_PushEvent(&event);

            return 0;
        }
        return -1;
    }
} // namespace JAZZROS
