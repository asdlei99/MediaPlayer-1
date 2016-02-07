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


#include <GLES2/gl2.h>
//ros: #include <SDL2/SDL.h>
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

struct myData {
    SDL_Window *window;
    SDL_GLContext context;
};

const GLchar vertex[] =
    "#version 100\n"
    "precision mediump float;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "   gl_PointSize = 128.0;\n"
    "}\0";

const GLchar fragment[] =
    "#version 100\n"
    "precision mediump float;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\0";

GLuint loadShader(GLuint program, GLenum type, const GLchar *shaderSrc) {

    GLuint shader;

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    glAttachShader(program, shader);

    return 0;
}

// cleanup before quiting
static int
cleanup(myData *data, int rc)
{
    // TODO: delete texture objects

    // Print out some timing information
    /*
    now = SDL_GetTicks();
    if (now > then) {
        LOGE("%2.2f frames per second",
             ((double) frames * 1000) / (now - then));
    }
     */
    if(data->context)
        SDL_GL_DeleteContext(data->context);
    if(data->window)
        SDL_DestroyWindow(data->window);

    SDL_Quit();

    return rc;
}

int sdlInit(myData *data) {
#if 0
    SDL_Init(SDL_INIT_VIDEO);
#else // 0
    // Init the window, the GL context
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { /* Initialize SDL's Video subsystem */
        LOG("Unable to initialize SDL");
        std::string str (SDL_GetError());
        //str << "Error : " << SDL_GetError() << std::endl;
        const char * ptr = str.c_str();
        LOG("%s", ptr);
        return cleanup(data, -1);
    }
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    //CHECK_SDL();
#endif // 0
    //
    // ros: added by comment from stackoverflow
    //
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    //
    //
    //
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);



//    data->window = SDL_CreateWindow("Demo", 0, 0, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    data->window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mode.w, mode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!data->window) {
        // Die if creation failed
        LOG("Unable to create window");
        return cleanup(data, -1);
    }
    SDL_SetWindowFullscreen(data->window, SDL_TRUE);
    //CHECK_SDL();

    data->context = SDL_GL_CreateContext(data->window);
    //CHECK_SDL();
    if (!data->context) {
        LOG("Unable to create GL context");
        return cleanup(data, -1);
    }


    return 0;
}

int glInit(myData *data) {
    GLuint programObject;

    programObject = glCreateProgram();

    loadShader(programObject, GL_VERTEX_SHADER, vertex);
    loadShader(programObject, GL_FRAGMENT_SHADER, fragment);
    glLinkProgram(programObject);
    glUseProgram(programObject);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, 512, 512);

    return 0;
}

int loopFunc(myData *data) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, 1);
    SDL_GL_SwapWindow(data->window);

    return 0;
}
/*
void sdlQuit(myData *data) {
    SDL_GL_DeleteContext(data->context);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
    return;
}
*/

myData data;

int init ( ) {

    int sdlInitRes = sdlInit(&data);
    if (sdlInitRes < 0)
        return sdlInitRes;

    glInit(&data);

    //while (!loopFunc(&data));

    //sdlQuit(&data);

    return 0;
}


int loop ( ) {
    return loopFunc(&data);
}

int quit ( ) {
    return cleanup(&data, 0);
}

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

void SDL_setImage(const unsigned short &width, const unsigned short &height,
                  const int &someInt,
                  const GLint &interanlTexFormat,
                  const GLint &pixFormat,
                  unsigned char *pFramePtr) {
/*
    void *mPixels = NULL;
    int mPitch = 0;

    SDL_LockTexture(gdata.bitmapTex, NULL, reinterpret_cast<void **>(&mPixels), &mPitch);
    //SDL_memcpy(mPixels, gdata.pixels, gdata.w * gdata.h * 3);
    SDL_memcpy(mPixels, pFramePtr, gdata.w * gdata.h * 3);
    SDL_UnlockTexture(gdata.bitmapTex);

*/
/*
    int frame_size = gdata.w * gdata.h * 12/8;

    memcpy (gdata.pixels, pFramePtr, frame_size);
*/
}

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

int nativePlayerOpen(const char* mfileName);
const int   gPlayerPlay();

#ifdef __cplusplus
}
#endif


// need for sdl2 static
int main( int argc, char *argv[] ) {
    LOG("Calling method main() and init()");

    /*
    init();

    while (!loopFunc(&data));

    quit();
     */
    #define JAZZROS

    SDL_Window *window = NULL;
    // SDL_Surface *bitmapSurface = NULL;

    int running = 1;

    LOG("started");

    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER);

    if (argc > 1) {
//        draw_interrupter = SDL_updateYUVTexture;

        LOG("nativePlayerOpen(%s)", argv[1]);
        nativePlayerOpen(argv[1]);

        gPlayerPlay();
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
                            break;
                        }
                    };
                    break;
                }
                case SDL_USEREVENT : {
                    /* and now we can call the function we wanted to call in the timer but couldn't because of the multithreading problems */

                    void (*p)(void *) = (void (*)(void *)) event.user.data1;
                    if (p)
                        p(event.user.data2);

                    break;
                }
            }
        }
    }

    LOG("finished");

//    SDL_DestroyTexture(bitmapTex);
//    SDL_DestroyRenderer(renderer);
//    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
