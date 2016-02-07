#include "VideoOutputDeviceSDL.hpp"

namespace JAZZROS {

    class VideoOutputDeviceDataSDL : public VideoOutputDeviceData {
    private:
        virtual const int Initialize(const FFmpegFileHolder & pFileHolder) {

            /**
             * Initialize parent class' object
             */
            VideoOutputDeviceData::Initialize(pFileHolder);

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
            return 0;
        }
    public:
        int     m_sdlPixelFormat;
        VideoOutputDeviceDataSDL()
                :VideoOutputDeviceData(AV_PIX_FMT_YUV420P)
                //:VideoOutputDeviceData(AV_PIX_FMT_RGB565LE)
        {}
    };

    VideoOutputDeviceSDL::VideoOutputDeviceSDL()
            : VideoOutputDevice(new VideoOutputDeviceDataSDL()) {
    }

    VideoOutputDeviceSDL::~VideoOutputDeviceSDL() {
        SDL_DestroyTexture(bitmapTex);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    const int
    VideoOutputDeviceSDL::Initialize (VideoOutputDeviceData *pVODD, const FFmpegFileHolder & pFileHolder) {

        if (pVODD == NULL)
            return -1;

        if (pVODD->Initialize(pFileHolder) != 0)
            return -2;

        // local initialization
        const VideoOutputDeviceDataSDL * pData     = dynamic_cast<const VideoOutputDeviceDataSDL*>(getData());

        const Size frameSize = pData->getFrameSize();
        const int  sdlPixFmt = pData->m_sdlPixelFormat;

        window = SDL_CreateWindow("SDL Media Player Window",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  640, 480, SDL_WINDOW_SHOWN);
        if (window == NULL)
            return -1;

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
            return -1;

        bitmapTex = SDL_CreateTexture(renderer,
                                      sdlPixFmt,
                                      SDL_TEXTUREACCESS_STREAMING/*texture will be able modifying*/,
                                      frameSize.Width,
                                      frameSize.Height);
        if (bitmapTex == NULL)
            return -1;

        return 0;
    }

    struct sParam {
        VideoOutputDeviceSDL * pThis;
        const unsigned char * pFramePtr;
    };

    void my_function(void *pParam) {

        sParam * param = (sParam *)pParam;

        VideoOutputDeviceSDL * pThis = param->pThis;


        const VideoOutputDeviceDataSDL * pData     = dynamic_cast<const VideoOutputDeviceDataSDL*>(pThis->getData());

        const unsigned char * pFramePtr = (const unsigned char *)(param->pFramePtr);

        void *mPixels = NULL;
        int mPitch = 0;
        unsigned int frame_size = pData->getMemoeryFrameSize();

        SDL_LockTexture(pThis->bitmapTex, NULL, reinterpret_cast<void **>(&mPixels), &mPitch);
        SDL_memcpy(mPixels, pFramePtr, frame_size);
        SDL_UnlockTexture(pThis->bitmapTex);

        SDL_RenderClear(pThis->renderer);
        SDL_RenderCopy(pThis->renderer, pThis->bitmapTex, NULL, NULL);
        SDL_RenderPresent(pThis->renderer);
    }

    const int
    VideoOutputDeviceSDL::render(const unsigned char * pFramePtr) {

        const VideoOutputDeviceDataSDL * pData     = dynamic_cast<const VideoOutputDeviceDataSDL*>(getData());

        if (pData) {
            static sParam param;
            //
            param.pThis = this;
            param.pFramePtr = pFramePtr;
            /**
             *
             */
            SDL_Event       event;
            SDL_UserEvent   userevent;

            userevent.type = SDL_USEREVENT;
            userevent.code = 0;
            userevent.data1 = (void *)(& my_function);;
            userevent.data2 = (void *)(&param);

            event.type = SDL_USEREVENT;
            event.user = userevent;

            SDL_PushEvent(&event);

            return 0;
        }
        return -1;
    }
} // namespace JAZZROS