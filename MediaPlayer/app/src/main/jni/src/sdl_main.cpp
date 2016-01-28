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

void my_function(void *param) {
    my_struct *my_param = (my_struct *) param;
    //
    unsigned char step = 5;
    my_param->red += step;
    my_param->green += step;
    my_param->blue += step;
    for (int y = 0; y < my_param->h / 2; y++) {
        for (int x = 0; x < my_param->w / 2; x++) {
            //colors from http://www.conservativespace.com/ColorPicker.php
            //Color AARRGGBB
            //pixels[x+(y*w)] = color;
            my_param->pixels[(x + (y * my_param->w)) * 3 + 0] = my_param->red;
            my_param->pixels[(x + (y * my_param->w)) * 3 + 1] = my_param->green;
            my_param->pixels[(x + (y * my_param->w)) * 3 + 2] = my_param->blue;
        }
    }
    //
    //
    //
    void *mPixels = NULL;
    int mPitch = 0;

    SDL_LockTexture(my_param->bitmapTex, NULL, reinterpret_cast<void **>(&mPixels), &mPitch);
    SDL_memcpy(mPixels, my_param->pixels, my_param->w * my_param->h * 3);
    SDL_UnlockTexture(my_param->bitmapTex);
}


/* with the same code as before: */
Uint32 my_callbackfunc(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes a function
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
//    userevent.data1 = &my_function;
    userevent.data1 = (void *)(& my_function);
    userevent.data2 = param;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
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
    SDL_Renderer *renderer = NULL;
    SDL_Texture *bitmapTex = NULL;
    // SDL_Surface *bitmapSurface = NULL;

    int running = 1;

    LOG("started");

    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER);

    if (argc > 1) {
        LOG("nativePlayerOpen(%s)", argv[1]);
        nativePlayerOpen(argv[1]);

        gPlayerPlay();
    }

    window = SDL_CreateWindow("SDL Hello World",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              640, 480, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    const unsigned int w = 100;
    const unsigned int h = 100;

    unsigned char *pixels = new unsigned char[w*h*3];

    bitmapTex = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING/*texture will be able modifying*/,w,h);

    my_struct   data;
    data.bitmapTex = bitmapTex;
    data.red = 0;
    data.green = 128;
    data.blue = 255;
    data.w = w;
    data.h = h;
    data.pixels = pixels;
    //
    Uint32 delay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, my_callbackfunc, & data);
    //
    while (running) {
        SDL_Event event;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
        SDL_RenderPresent(renderer);

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
                    p(event.user.data2);
                    break;
                }
            }
        }
    }

    LOG("finished");

    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
