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

// Android Drag&Drop listview
// https://github.com/terlici/DragNDropList

#ifdef ANDROID
    #include <GLES2/gl2.h>
#else // ANDROID
    #include <GL/gl.h>
#endif

#include <SDL.h>
#include <SDL_main.h>

#include <algorithm>

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
#include "../libsrc/devices/VideoOutputDeviceSDL.hpp" // todo: only for SDL_USEREVENT_CODE_TIMER1
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

const int   gPlayerInit();
const int   gPlayerRelease();
const int   gPlayerOpen(const char * mfileName);
const int   gPlayerPlay(const int & index);
const int   gPlayerPause(const int & index);
const int   gPlayerSeek(const int & index, const double & percent);
const int   gPlayerSeekMS(const int & index, const int & ms);
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
    userevent.code = SDL_USEREVENT_CODE_TIMER1;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

    return(interval);
}

struct MediaFileDescriptor {
    int index;
    int startMS;
    int endMS;
    std::string filename;
};

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
//    if (itr != end && ++itr != end)
    if (itr != end)
    {
        return *++itr;
    }
    return NULL;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/**
    N<Number of media files>
    File1
    File2
    ...
    FileN
    -i
    <index of media file>
    -s
    <start MS>
    -e
    <end MS>
*/
// need for sdl2 static
int main( int argc, char *argv[] ) {

    std::vector<std::string> fileNameList;
    std::vector<MediaFileDescriptor> mediaFileDescriptors;
    std::size_t mediaFilesNb = 0;
    if (argc > 1)
        mediaFilesNb = atoi (argv[1]);

    if (argc <= 1 + mediaFilesNb) {
        LOG("Wrong params format");
    }

    int curPlayerIndex = -1;
    int playerIndex[128];
    SDL_TimerID timer_id = 0;

    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER);
    if (gPlayerInit() != 0) {// Only after SDL has been initialized
        LOG("Cannot initialize player");
        return -1;
    }

    for (int i = 0; i < mediaFilesNb; ++i) {
        char * pFileName = argv[2 + i];
        fileNameList.push_back(pFileName);
        playerIndex[i] = (i == 0) ? nativePlayerOpen(pFileName) : gPlayerOpen(pFileName);
        curPlayerIndex = playerIndex[i];
        if (curPlayerIndex < 0) {
            LOG("Cannot open media file %s", argv[2 + i]);
            return -1;
        }
    }
    curPlayerIndex = 0;

    int arg_par_index_0 = 2 + mediaFilesNb;
    int arg_par_index_1 = arg_par_index_0 + 1;
    while (arg_par_index_0 < argc &&
           arg_par_index_1 < argc &&
           cmdOptionExists(argv + arg_par_index_0, argv + arg_par_index_1, "-i"))
    {
        const int fileIndex = atoi(getCmdOption(argv + arg_par_index_0, argv + arg_par_index_1, "-i"));
        int startMS;
        int endMS;
        if (fileIndex >= mediaFilesNb) {
            LOG ("Wrong indexing of opened media");
            return -1;
        }
        arg_par_index_0 = arg_par_index_1 + 1;
        arg_par_index_1 = arg_par_index_0 + 1;
        if (arg_par_index_0 < argc &&
            arg_par_index_1 < argc &&
            cmdOptionExists(argv + arg_par_index_0, argv + arg_par_index_1, "-s")) {
                startMS = atoi(getCmdOption(argv + arg_par_index_0, argv + arg_par_index_1, "-s"));
        } else {
            LOG ("Missing params for playback media");
            return -1;
        }
        arg_par_index_0 = arg_par_index_1 + 1;
        arg_par_index_1 = arg_par_index_0 + 1;
        if (arg_par_index_0 < argc &&
            arg_par_index_1 < argc &&
            cmdOptionExists(argv + arg_par_index_0, argv + arg_par_index_1, "-e")) {
                endMS = atoi(getCmdOption(argv + arg_par_index_0, argv + arg_par_index_1, "-e"));
        } else {
            LOG ("Missing params for playback media");
            return -1;
        }
        arg_par_index_0 = arg_par_index_1 + 1;
        arg_par_index_1 = arg_par_index_0 + 1;
        //
        MediaFileDescriptor descr;
        descr.index = playerIndex[fileIndex];
        descr.startMS = startMS;
        descr.endMS = endMS;
        descr.filename = fileNameList[fileIndex];
        mediaFileDescriptors.push_back(descr);
    }


    LOG("Calling method main()");

    int running = 1;


    if (argc > 1) {
//        draw_interrupter = SDL_updateYUVTexture;
/*
        playerIndex0 = nativePlayerOpen(argv[1]);
        curPlayerIndex = playerIndex0;

        if (argc > 2)
            playerIndex1 = gPlayerOpen(argv[2]);

        timer_id = SDL_AddTimer(3000, timerFunctionAnotherThread, NULL);

        gPlayerPlay (playerIndex0);

        if (playerIndex1 >= 0) {
            gPlayerPlay (playerIndex1);
            curPlayerIndex = playerIndex1;
        }
*/
        MediaFileDescriptor & descr = mediaFileDescriptors[curPlayerIndex];
        gPlayerSeekMS(descr.index,
                      descr.startMS);
        gPlayerPlay (descr.index);
        timer_id = SDL_AddTimer(descr.endMS -
                                descr.startMS,
                                timerFunctionAnotherThread,
                                NULL);
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
/*
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
*/
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
                        case SDL_USEREVENT_CODE_RENDER:
                        {
                            void (*p)(void *) = (void (*)(void *)) event.user.data1;
                            if (p)
                                p(event.user.data2);
                            break;
                        }
                        case SDL_USEREVENT_CODE_TIMER1:
                        {
                            gPlayerPause (mediaFileDescriptors[curPlayerIndex].index);
                            curPlayerIndex++;
                            if (curPlayerIndex < mediaFileDescriptors.size()) {
                                SDL_RemoveTimer (timer_id);
                                MediaFileDescriptor & descr = mediaFileDescriptors[curPlayerIndex];

                                gPlayerSeekMS(descr.index,
                                              descr.startMS);
                                gPlayerPlay (descr.index);
                                timer_id = SDL_AddTimer(descr.endMS -
                                                        descr.startMS,
                                                        timerFunctionAnotherThread,
                                                        NULL);
                                if (timer_id == 0) {
                                    LOG ("ERROR in timing processes");
                                    return -1;
                                }
                            } else {
                                running = 0;
                            }

                            break;
                        }
                        case SDL_USEREVENT_CODE_PLAYERFINISHED:
                        {
                            static int quitPlayerNb = 0;
                            const char * quitPlayerFileName = reinterpret_cast<const char *>(event.user.data1);
                            int quitPlayerIndex = -1;

                            LOG ("Playback finish signal received: %s", quitPlayerFileName);

                            for (int i = 0; i + 1 < argc; ++i)
                            {
                                if (strcmp(quitPlayerFileName, argv[i + 2]) == 0)
                                {
                                    quitPlayerIndex = i;
                                    break;
                                }
                            }
                            if (quitPlayerIndex >= 0)
                            {
                                LOG ("Playback %d quit", quitPlayerIndex);
                                gPlayerQuit (quitPlayerIndex);
                                quitPlayerNb++;
//                                if (quitPlayerNb + 1 == argc)
                                if (quitPlayerNb == mediaFileDescriptors.size())
                                {
                                    running = 0;
                                }
                            }
                            else
                            {
                                LOG ("Playback finish signal does not recognized");
                            }
                            break;
                        }
                    };

                    break;
                }
            }
        }
    }

    LOG("Method main() finished");

    if (timer_id != 0)
        SDL_RemoveTimer (timer_id);

    for (int i = 0; i < mediaFileDescriptors.size(); ++i)
        gPlayerQuit(mediaFileDescriptors[i].index);

    gPlayerRelease(); // Just before SDL quit
    SDL_Quit();
    LOG("SDL_Quit");

    return 0;
}

