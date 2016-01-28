#include "game.h"
#include "com_jazzros_ffmpegtest_RendererWrapper.h"

//#include "glwrapper.h"
#include <GLES2/gl2.h>
#include <pthread.h>

void on_surface_created() {
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
}

void on_surface_changed(int w, int h) {
    // No-op
}

void on_draw_frame() {
    glClear(GL_COLOR_BUFFER_BIT);
}

// source link
// http://stackoverflow.com/questions/26534304/android-jni-call-attachcurrentthread-without-detachcurrentthread

static JavaVM* jvm = 0;
static jobject activity = 0; // GlobalRef

void* threadFunction(void* irrelevant)
{
    JNIEnv* env;
    usleep(5000000);

    jint res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    if (res >= 0) {
        jclass clazz = (*env)->GetObjectClass(env, activity);
        jmethodID methodID = (*env)->GetMethodID(env, clazz, "test", "()V" );

        (*env)->CallVoidMethod(env, activity, methodID);

        (*jvm)->DetachCurrentThread(jvm);
    }
}

JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_RendererWrapper_on_1surface_1created(JNIEnv * env, jobject thiz)
{
    on_surface_created();

/*
    JavaVM* jvm = 0;
    jobject activity = 0; // GlobalRef

    (*env)->GetJavaVM(env, &jvm);
    activity = (*env)->NewGlobalRef(env, thiz);

    jint res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    if (res >= 0) {
        jclass clazz = (*env)->GetObjectClass(env, activity);
        jmethodID methodID = (*env)->GetMethodID(env, clazz, "test", "()V" );

        (*env)->CallVoidMethod(env, activity, methodID);
    }
*/
    (*env)->GetJavaVM(env, &jvm);
    activity = (*env)->NewGlobalRef(env, thiz);

    pthread_t hThread;
    pthread_create(&hThread, NULL, &threadFunction, NULL);
}

/*
 * Class:     com_jazzros_ffmpegtest_RendererWrapper
 * Method:    on_surface_changed
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_RendererWrapper_on_1surface_1changed(JNIEnv * env, jobject obj, jint w, jint h)
{
    on_surface_changed(w,h);
}

/*
 * Class:     com_jazzros_ffmpegtest_RendererWrapper
 * Method:    on_draw_frame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_jazzros_ffmpegtest_RendererWrapper_on_1draw_1frame (JNIEnv * env, jobject obj)
{
    on_draw_frame();
}