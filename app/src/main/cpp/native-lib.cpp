#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "FFNative"
#define ALOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define ALOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))



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

typedef struct {
    char factor;
    char *value;
    int length;
    char decoded;
} XHBGLESShaderData;

/* 解析出混淆的值 */
const char *xhb_GLESCString(const XHBGLESShaderData *data) {
    if (data->decoded == 1)
        return data->value;
    for (int i = 0; i < data->length; i++) {
        data->value[i] ^= data->factor;
    }
    ((XHBGLESShaderData *) data)->decoded = 1;
    return data->value;
}


/* 顶点着色器*/
static XHBGLESShaderData xhb_vs = {
        .factor = (char)38,
        .value = (char []){(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)71,(char)82,(char)82,(char)84,(char)79,(char)68,(char)83,(char)82,(char)67,(char)6,(char)80,(char)67,(char)69,(char)21,(char)6,(char)86,(char)73,(char)85,(char)79,(char)82,(char)79,(char)73,(char)72,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)71,(char)82,(char)82,(char)84,(char)79,(char)68,(char)83,(char)82,(char)67,(char)6,(char)80,(char)67,(char)69,(char)20,(char)6,(char)79,(char)72,(char)114,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)23,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)71,(char)82,(char)82,(char)84,(char)79,(char)68,(char)83,(char)82,(char)67,(char)6,(char)80,(char)67,(char)69,(char)20,(char)6,(char)79,(char)72,(char)114,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)20,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)80,(char)71,(char)84,(char)95,(char)79,(char)72,(char)65,(char)6,(char)80,(char)67,(char)69,(char)20,(char)6,(char)82,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)23,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)80,(char)71,(char)84,(char)95,(char)79,(char)72,(char)65,(char)6,(char)80,(char)67,(char)69,(char)20,(char)6,(char)82,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)20,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)80,(char)73,(char)79,(char)66,(char)6,(char)75,(char)71,(char)79,(char)72,(char)14,(char)15,(char)6,(char)93,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)65,(char)74,(char)121,(char)118,(char)73,(char)85,(char)79,(char)82,(char)79,(char)73,(char)72,(char)6,(char)27,(char)6,(char)80,(char)67,(char)69,(char)18,(char)14,(char)86,(char)73,(char)85,(char)79,(char)82,(char)79,(char)73,(char)72,(char)10,(char)6,(char)23,(char)8,(char)22,(char)15,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)82,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)23,(char)6,(char)27,(char)6,(char)79,(char)72,(char)114,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)23,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)82,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)20,(char)6,(char)27,(char)6,(char)79,(char)72,(char)114,(char)67,(char)94,(char)69,(char)73,(char)73,(char)84,(char)66,(char)20,(char)29,(char)44,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)6,(char)91,0},
        .length = 321
};

const XHBGLESShaderData * const _3758971219 = &xhb_vs;

/* 片段着色器 */
static XHBGLESShaderData xhb_fs = {
        .factor = (char)108,
        .value = (char []){(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)28,(char)30,(char)9,(char)15,(char)5,(char)31,(char)5,(char)3,(char)2,(char)76,(char)1,(char)9,(char)8,(char)5,(char)25,(char)1,(char)28,(char)76,(char)10,(char)0,(char)3,(char)13,(char)24,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)13,(char)30,(char)21,(char)5,(char)2,(char)11,(char)76,(char)26,(char)9,(char)15,(char)94,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)93,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)13,(char)30,(char)21,(char)5,(char)2,(char)11,(char)76,(char)26,(char)9,(char)15,(char)94,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)94,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)25,(char)2,(char)5,(char)10,(char)3,(char)30,(char)1,(char)76,(char)31,(char)13,(char)1,(char)28,(char)0,(char)9,(char)30,(char)94,(char)40,(char)76,(char)21,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)25,(char)2,(char)5,(char)10,(char)3,(char)30,(char)1,(char)76,(char)31,(char)13,(char)1,(char)28,(char)0,(char)9,(char)30,(char)94,(char)40,(char)76,(char)25,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)25,(char)2,(char)5,(char)10,(char)3,(char)30,(char)1,(char)76,(char)31,(char)13,(char)1,(char)28,(char)0,(char)9,(char)30,(char)94,(char)40,(char)76,(char)26,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)3,(char)5,(char)8,(char)76,(char)1,(char)13,(char)5,(char)2,(char)68,(char)69,(char)76,(char)23,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)9,(char)15,(char)88,(char)76,(char)15,(char)76,(char)81,(char)76,(char)26,(char)9,(char)15,(char)88,(char)68,(char)68,(char)24,(char)9,(char)20,(char)24,(char)25,(char)30,(char)9,(char)94,(char)40,(char)68,(char)21,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)64,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)93,(char)69,(char)66,(char)30,(char)76,(char)65,(char)76,(char)93,(char)90,(char)66,(char)76,(char)67,(char)76,(char)94,(char)89,(char)89,(char)66,(char)69,(char)76,(char)70,(char)76,(char)93,(char)66,(char)93,(char)90,(char)88,(char)69,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)9,(char)15,(char)88,(char)76,(char)57,(char)76,(char)81,(char)76,(char)26,(char)9,(char)15,(char)88,(char)68,(char)24,(char)9,(char)20,(char)24,(char)25,(char)30,(char)9,(char)94,(char)40,(char)68,(char)25,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)64,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)93,(char)69,(char)66,(char)30,(char)76,(char)65,(char)76,(char)93,(char)94,(char)84,(char)66,(char)76,(char)67,(char)76,(char)94,(char)89,(char)89,(char)66,(char)69,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)26,(char)9,(char)15,(char)88,(char)76,(char)58,(char)76,(char)81,(char)76,(char)26,(char)9,(char)15,(char)88,(char)68,(char)24,(char)9,(char)20,(char)24,(char)25,(char)30,(char)9,(char)94,(char)40,(char)68,(char)26,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)64,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)93,(char)69,(char)66,(char)30,(char)76,(char)65,(char)76,(char)93,(char)94,(char)84,(char)66,(char)76,(char)67,(char)76,(char)94,(char)89,(char)89,(char)66,(char)69,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)15,(char)76,(char)71,(char)81,(char)76,(char)58,(char)76,(char)70,(char)76,(char)26,(char)9,(char)15,(char)88,(char)68,(char)93,(char)66,(char)89,(char)85,(char)90,(char)64,(char)76,(char)65,(char)92,(char)66,(char)84,(char)93,(char)95,(char)64,(char)76,(char)92,(char)64,(char)76,(char)92,(char)69,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)15,(char)76,(char)71,(char)81,(char)76,(char)57,(char)76,(char)70,(char)76,(char)26,(char)9,(char)15,(char)88,(char)68,(char)92,(char)64,(char)76,(char)65,(char)92,(char)66,(char)95,(char)85,(char)94,(char)64,(char)76,(char)94,(char)66,(char)92,(char)93,(char)91,(char)64,(char)76,(char)92,(char)69,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)15,(char)66,(char)13,(char)76,(char)81,(char)76,(char)24,(char)9,(char)20,(char)24,(char)25,(char)30,(char)9,(char)94,(char)40,(char)68,(char)21,(char)60,(char)0,(char)13,(char)2,(char)9,(char)56,(char)9,(char)20,(char)64,(char)76,(char)24,(char)9,(char)20,(char)15,(char)3,(char)3,(char)30,(char)8,(char)94,(char)69,(char)66,(char)30,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)11,(char)0,(char)51,(char)42,(char)30,(char)13,(char)11,(char)47,(char)3,(char)0,(char)3,(char)30,(char)76,(char)81,(char)76,(char)15,(char)87,(char)102,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)76,(char)17,0},
        .length = 655
};


const XHBGLESShaderData * const _3800906352 = &xhb_fs;

jmethodID javaMethodFieldId;

bool play = true;

extern "C"
JNIEXPORT void JNICALL
Java_com_ffmpeg_bbeffect_BbEffectView_initEffectPlay(
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
Java_com_ffmpeg_bbeffect_BbEffectView_videoStop(
        JNIEnv *env,
        jobject instance) {
    play = false;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_ffmpeg_bbeffect_BbEffectView_videoPlay(JNIEnv *env, jobject instance, jstring path_,
                                              jobject surface) {
    const char *path = env->GetStringUTFChars(path_, 0);

    /***
     * ffmpeg 初始化
     * **/
    av_register_all();
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (avformat_open_input(&fmt_ctx, path, NULL, NULL) < 0) {
        // 特效资源不存在
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 300);
        }
        return;
    }
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        // 没有找到流信息 文件损坏
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 400);
        }
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
        // 没有找到视频流 文件损坏
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 500);
        }
        return;
    }
    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec_ctx, avStream->codecpar);
    AVCodec *avCodec = avcodec_find_decoder(codec_ctx->codec_id);
    if (avcodec_open2(codec_ctx, avCodec, NULL) < 0) {
        // 解码器初始化失败
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 600);
        }
        return;
    }
    play = true;

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

    if (eglCtx == EGL_NO_CONTEXT) {
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 800);
        }
        return;
    }
    int makeCurrentResult = eglMakeCurrent(eglDisp, eglWindow, eglWindow, eglCtx);
    ALOGI("%s%d","eglMakeCurrent: ",makeCurrentResult);
    if (makeCurrentResult == EGL_FALSE) {
        if (javaMethodFieldId != NULL) {
            env->CallVoidMethod(instance, javaMethodFieldId, 6, 900);
        }
        return;
    }
    ALOGI("%s%s%d%s%d","setBuffersGeometry","windowWidth",windowWidth,"windowHeight",windowHeight);
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

    GLuint programId = shaderUtils->createProgram(xhb_GLESCString(_3758971219), xhb_GLESCString(_3800906352));
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

    // 开始播放动效
    if (javaMethodFieldId != NULL) {
        env->CallVoidMethod(instance, javaMethodFieldId, 4, 100);
    }

    /***
     * 开始解码
     * **/
    int ret;
    while (play) {
        if (av_read_frame(fmt_ctx, pkt) < 0) {
            //播放结束
            break;
        }
        usleep(30000);
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
    /**
     * 结束播放回调
     */
    if (javaMethodFieldId != NULL) {
        env->CallVoidMethod(instance, javaMethodFieldId, 4, 200);
    }
    env->ReleaseStringUTFChars(path_, path);
}