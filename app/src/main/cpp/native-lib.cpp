#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "FFNative"
#define ALOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define ALOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_ffmpeg_bbeffect_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

#include "ShaderUtils.h"
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include <unistd.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavfilter/avfilter.h>
}

#define GET_STR(x) #x
const char *vertexShaderString = GET_STR(
        attribute vec3 position;
        attribute vec2 inTexcoord1;
        attribute vec2 inTexcoord2;
        varying vec2 texcoord1;
        varying vec2 texcoord2;
        void main() {
            gl_Position = vec4(position, 1.0);
            texcoord1 = inTexcoord1;
            texcoord2 = inTexcoord2;
        }
);
const char *fragmentShaderString = GET_STR(
        precision mediump float;
        varying vec2 texcoord1;
        varying vec2 texcoord2;
        uniform sampler2D yPlaneTex;
        uniform sampler2D uPlaneTex;
        uniform sampler2D vPlaneTex;
        void main() {
            //因为是YUV的一个平面，所以采样后的r,g,b,a这四个参数的数值是一样的 所以这里统一取r即可
            vec4 c = vec4((texture2D(yPlaneTex, texcoord1).r - 16. / 255.) * 1.164);
            vec4 U = vec4(texture2D(uPlaneTex, texcoord1).r - 128. / 255.);
            vec4 V = vec4(texture2D(vPlaneTex, texcoord1).r - 128. / 255.);
            c += V * vec4(1.596, -0.813, 0, 0);
            c += U * vec4(0, -0.392, 2.017, 0);
            c.a = texture2D(yPlaneTex, texcoord2).r;
            gl_FragColor = c;
        }
);

jmethodID javaMethodFieldId;


extern "C"
JNIEXPORT void JNICALL
Java_com_ffmpeg_bbeffect_MainActivity_initEffectPlay(
        JNIEnv *env,
        jobject instance) {
    //获得Java层该对象实例的类引用，即MainActivity类引用
    jclass activityClass = env->GetObjectClass(instance);

    //获取到方法的(句柄)域ID
    if (javaMethodFieldId == NULL) {
        javaMethodFieldId = env->GetMethodID(activityClass, "onAnimEvent", "(II)V");
    }
}



extern "C"
JNIEXPORT void JNICALL
Java_com_ffmpeg_bbeffect_MainActivity_videoPlay(JNIEnv *env, jobject instance, jstring path_,
                                                jobject surface) {
    const char *path = env->GetStringUTFChars(path_, 0);

    if (javaMethodFieldId != NULL) {
        env->CallObjectMethod(instance, javaMethodFieldId, 110, 220);
    }

    /***
     * ffmpeg 初始化
     * **/
    av_register_all();
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (avformat_open_input(&fmt_ctx, path, NULL, NULL) < 0) {
        return;
    }
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        return;
    }
    AVStream *avStream = NULL;
    int video_stream_index = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            avStream = fmt_ctx->streams[i];
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        return;
    }
    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec_ctx, avStream->codecpar);
    AVCodec *avCodec = avcodec_find_decoder(codec_ctx->codec_id);
    if (avcodec_open2(codec_ctx, avCodec, NULL) < 0) {
        return;
    }
    int y_size = codec_ctx->width * codec_ctx->height;
    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket));
    av_new_packet(pkt, y_size);

    /**
    *初始化egl
    **/
    EGLConfig eglConf;
    EGLSurface eglWindow;
    EGLContext eglCtx;
    int windowWidth = codec_ctx->width;
    int windowHeight = codec_ctx->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    EGLint configSpec[] = {EGL_RED_SIZE, 8,
                           EGL_GREEN_SIZE, 8,
                           EGL_BLUE_SIZE, 8,
                           EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE};

    EGLDisplay eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;
    eglInitialize(eglDisp, &eglMajVers, &eglMinVers);
    eglChooseConfig(eglDisp, configSpec, &eglConf, 1, &numConfigs);

    eglWindow = eglCreateWindowSurface(eglDisp, eglConf, nativeWindow, NULL);

    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);

    eglMakeCurrent(eglDisp, eglWindow, eglWindow, eglCtx);

    ANativeWindow_setBuffersGeometry(nativeWindow, windowWidth, windowHeight,
                                     WINDOW_FORMAT_RGBA_8888);

    /**
     * 设置opengl 要在egl初始化后进行
     * **/
    float *vertexData = new float[12]{
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };

    float *textureVertexData = new float[8]{
            1.0f, 0.5f,//右下
            0.0f, 0.5f,//左下
            1.0f, 0.0f,//右上
            0.0f, 0.0f//左上
    };

    float *textureVertexDataBottom = new float[8]{
            1.0, 1.0,//右下
            0.0, 1.0,//左下
            1.0, 0.5,//右上
            0.0, 0.5//左上
    };

    ShaderUtils *shaderUtils = new ShaderUtils();

    GLuint programId = shaderUtils->createProgram(vertexShaderString, fragmentShaderString);
    delete shaderUtils;
    GLuint position = (GLuint) glGetAttribLocation(programId, "position");
    GLuint texcoord1 = (GLuint) glGetAttribLocation(programId, "inTexcoord1");
    GLuint texcoord2 = (GLuint) glGetAttribLocation(programId, "inTexcoord2");

    GLint yPlaneTex = (GLint) glGetUniformLocation(programId, "yPlaneTex");
    GLint uPlaneTex = (GLint) glGetUniformLocation(programId, "uPlaneTex");
    GLint vPlaneTex = (GLint) glGetUniformLocation(programId, "vPlaneTex");
    glViewport(0, 0, windowWidth + 25, windowHeight);

    glUseProgram(programId);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 12, vertexData);

    glEnableVertexAttribArray(texcoord1);
    glVertexAttribPointer(texcoord1, 2, GL_FLOAT, GL_FALSE, 8, textureVertexData);

    glEnableVertexAttribArray(texcoord2);
    glVertexAttribPointer(texcoord2, 2, GL_FLOAT, GL_FALSE, 8, textureVertexDataBottom);

    /***
     * 初始化空的yuv纹理
     * **/
    GLuint yTextureId;
    GLuint uTextureId;
    GLuint vTextureId;
    glGenTextures(1, &yTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glUniform1i(yPlaneTex, 0);

    glGenTextures(1, &uTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glUniform1i(uPlaneTex, 1);

    glGenTextures(1, &vTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glUniform1i(vPlaneTex, 2);


    /***
     * 开始解码
     * **/
    int ret;
    while (1) {
        usleep(30000);
        if (av_read_frame(fmt_ctx, pkt) < 0) {
            //播放结束
            break;
        }
        if (pkt->stream_index == video_stream_index) {
            ret = avcodec_send_packet(codec_ctx, pkt);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                av_packet_unref(pkt);
                continue;
            }
            AVFrame *yuvFrame = av_frame_alloc();
            ret = avcodec_receive_frame(codec_ctx, yuvFrame);
            if (ret < 0 && ret != AVERROR_EOF) {
                av_frame_free(&yuvFrame);
                av_packet_unref(pkt);
                continue;
            }
            /***
              * 解码后的数据更新到yuv纹理中
            * **/

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, yTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[0], yuvFrame->height, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvFrame->data[0]);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, uTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[1],
                         (yuvFrame->height / 2) - 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         yuvFrame->data[1]);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, vTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[2],
                         (yuvFrame->height / 2) - 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         yuvFrame->data[2]);


            /***
            * 纹理更新完成后开始绘制
            ***/
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            eglSwapBuffers(eglDisp, eglWindow);

            glDisable(GL_BLEND);

            av_frame_free(&yuvFrame);
        }
        av_packet_unref(pkt);
    }
    /***
     * 释放资源
     * **/
    delete vertexData;
    delete textureVertexData;

    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglWindow);
    eglTerminate(eglDisp);
    eglDisp = EGL_NO_DISPLAY;
    eglWindow = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;

    avcodec_close(codec_ctx);
    avformat_close_input(&fmt_ctx);

    env->ReleaseStringUTFChars(path_, path);
}