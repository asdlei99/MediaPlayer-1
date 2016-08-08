/** source link */

// sdl2 & opengl es 2
// http://stackoverflow.com/questions/21621054/why-wont-this-simple-opengl-es-2-0-sdl-2-program-let-me-change-my-point-sprite
// - https://github.com/batiste/sdl2-opengl-es
// http://stackoverflow.com/questions/25771735/creating-opengl-texture-from-sdl2-surface-strange-pixel-values
// - https://zserge.wordpress.com/2012/02/27/androidsdl-%D0%B1%D1%83%D0%B4%D1%8C-%D0%BC%D1%83%D0%B6%D0%B8%D0%BA%D0%BE%D0%BC-%D0%BF%D0%B8%D1%88%D0%B8-%D0%BD%D0%B0-c/
// - http://stackoverflow.com/questions/25892387/displaying-texture-in-android-using-opengl-es-2-0-and-sdl-2-0-in-cnative-code

// mediacodec
// http://developer.android.com/intl/ru/reference/android/media/MediaCodec.html
// http://habrahabr.ru/post/233005/
// https://vec.io/posts/use-android-hardware-decoder-with-omxcodec-in-ndk
// http://blog.csdn.net/honemleysm/article/details/11485061
// https://github.com/vecio/MediaCodecDemo/blob/master/src/io/vec/demo/mediacodec/DecodeActivity.java
// http://stackoverflow.com/questions/32841513/mediacodec-crash-on-high-quality-stream
// http://android.vladli.com/2015/06/05/mediacodec-virtual-display-and-streaming/

// ffmpeg
// https://github.com/OnlyInAmerica/FFmpegTest/tree/master/jni

#ifdef ANDROID
    #include <GLES2/gl2.h>
#else // ANDROID
    #include <GL/gl.h>
#endif

#include <SDL.h>
#include <SDL_main.h>

#define  LOG_TAG    "SDL"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>

#define  LOG(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  LOGI(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  LOGE(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define  ASSETS_DIR ""

#else

#define  LOG(...)   printf(__VA_ARGS__);printf("\n");
#define  LOGI(...)  printf(__VA_ARGS__);printf("\n");
#define  LOGE(...)  printf(__VA_ARGS__);printf("\n");
#define  ASSETS_DIR "assets/"

#endif

#include <string>
#include "../libsrc/FFmpegPlayer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_AV_LOCK_MANAGER
    extern int (*pfLockMgr)(void **mutex, enum AVLockOp op);
    extern void initializePfLockMgr(void);
#endif // USE_AV_LOCK_MANAGER


void log_default_callback(void *ptr, int level, const char *fmt, va_list vargs)
{
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_INFO, "com.jazzros.ffmpegtest", fmt, vargs);
#else
    //printf(vargs);
#endif // ANDROID
}

#ifdef __cplusplus
} // extern "C"
#endif



struct my_struct {
    unsigned  char red;
    unsigned  char green;
    unsigned  char blue;

    unsigned int w;
    unsigned int h;

    unsigned char * pixels;

    SDL_Texture *bitmapTex;
};

my_struct   gdata;
SDL_Renderer *renderer = NULL;
SDL_Texture *bitmapTex = NULL;

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

void my_function(void *param) {
/*
    my_struct *my_param = (my_struct *) param;
    //
    int frame_size = gdata.w * gdata.h * 12/8;
    //
    //
    //
    void *mPixels = NULL;
    int mPitch = 0;

    SDL_LockTexture(my_param->bitmapTex, NULL, reinterpret_cast<void **>(&mPixels), &mPitch);
    SDL_memcpy(mPixels, my_param->pixels, frame_size);
    SDL_UnlockTexture(my_param->bitmapTex);
*/

/*
    SDL_UpdateYUVTexture(bitmapTex, NULL,
                         gplane0, gplane0s,
                         gplane1, gplane1s,
                         gplane2, gplane2s);
*/
}

#ifdef __cplusplus
extern "C" {
#endif

const int   gPlayerOpen(const char * mfileName);
const int   gPlayerPlay(const int & index);
const int   gPlayerPause(const int & index);
const int   gPlayerSelectCurrent(const int & index);
const int   gPlayerSeek(const int & index, const double & percent);
void        gPlayerQuit(const int & index);

int nativePlayerOpen(const char* mfileName)
{

    LOG("nativeOpenFromFile()");

#ifdef USE_AV_LOCK_MANAGER
    // enable thread locking
    initializePfLockMgr();
    av_lockmgr_register(pfLockMgr);
    LOG("av_lockmgr_register()");
#else
    LOG("av_lockmgr_register() not supported");
#endif

//    avcodec_init();
    av_register_all();
    avcodec_register_all();
    avdevice_register_all();
    av_log_set_callback (log_default_callback);

    LOG("test file %s", mfileName);

    return gPlayerOpen (mfileName);
}


#ifdef __cplusplus
}
#endif


Uint32 timerFunctionAnotherThread(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 1;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

    return(interval);
}

// need for sdl2 static
int main( int argc, char *argv[] ) {

    int curPlayerIndex = -1;
    int playerIndex0 = -1;
    int playerIndex1 = -1;

    LOG("Calling method main()");

    int running = 1;

    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER);

    if (argc > 1) {
//        draw_interrupter = SDL_updateYUVTexture;

        LOG("nativePlayerOpen(%s)", argv[1]);
        playerIndex0 = nativePlayerOpen(argv[1]);
        curPlayerIndex = playerIndex0;

//        LOG("nativePlayerOpen(%s)", argv[1]);
        playerIndex1 = gPlayerOpen(argv[2]);

        SDL_TimerID timer_id = SDL_AddTimer(3000,timerFunctionAnotherThread, NULL);

        gPlayerPlay (playerIndex0);

        if (playerIndex1 >= 0) {
            gPlayerPlay (playerIndex1);
            curPlayerIndex = playerIndex1;
        }
    }
    //
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_KEYDOWN : {
                    switch (event.key.keysym.scancode) {
                        case  SDL_SCANCODE_AC_BACK: {
                            LOG("pressed back button, terminating");
                            running = 0;
                            break;
                        }
                        case SDL_SCANCODE_1: {
                            LOG("Pressed button #1");
                            gPlayerSeek(curPlayerIndex, 0.0);
                            break;
                        }
                        case SDL_SCANCODE_2: {
                            LOG("Pressed button #2");
                            gPlayerSeek(curPlayerIndex, 20.0);
                            break;
                        }
                        case SDL_SCANCODE_3: {
                            LOG("Pressed button #3");
                            gPlayerSeek(curPlayerIndex, 70.0);
                            break;
                        }
                        case SDL_SCANCODE_4: {
                            LOG("Pressed button #4");
                            gPlayerSeek(curPlayerIndex, 99.0);
                            break;
                        }
                        case SDL_SCANCODE_5: {
                            LOG("Pressed button #5");
                            gPlayerSeek(curPlayerIndex, 101.0);
                            break;
                        }
                        case SDL_SCANCODE_P: {
                            LOG("Pause");
                            gPlayerPause(curPlayerIndex);
                            break;
                        }
                        case SDL_SCANCODE_S: {
                            LOG("Play");
                            gPlayerPlay(curPlayerIndex);
                            break;
                        }
                        case SDL_SCANCODE_N: {
                            curPlayerIndex = curPlayerIndex == 0 ? 1 : 0;
                            gPlayerSelectCurrent(curPlayerIndex);
                            break;
                        }
                        case  SDL_SCANCODE_Q: {
                            LOG("pressed Q-button, terminating");
                            running = 0;
                            break;
                        }
                    };
                    break;
                }
                case SDL_USEREVENT : {
                    /* and now we can call the function we wanted to call in the timer but couldn't because of the multithreading problems */
                    switch (event.user.code)
                    {
                        case 0:
                        {
                            void (*p)(void *) = (void (*)(void *)) event.user.data1;
                            if (p)
                                p(event.user.data2);
                            break;
                        }
                        case 1:
                        {
                            if (playerIndex1 != -1)
                            {
                                curPlayerIndex = curPlayerIndex == 0 ? 1 : 0;
                                gPlayerSelectCurrent(curPlayerIndex);
                            }
                            break;
                        }
                    };

                    break;
                }
            }
        }
    }

    LOG("finished");

    gPlayerQuit(playerIndex0);
    if (playerIndex1 >= 0)
        gPlayerQuit(playerIndex1);

    SDL_Quit();

    return 0;
}

