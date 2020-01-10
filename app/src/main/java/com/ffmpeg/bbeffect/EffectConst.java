package com.ffmpeg.bbeffect;

/**
 * @auther SuZhanFeng
 * @date 2020/1/10
 * @desc
 */
public class EffectConst {

    /**
     * 消息分类为信息
     */
    public static int MSG_TYPE_INFO = 4;

    /**
     * 特效播放开始
     */
    public static final int MSG_STAT_EFFECTS_START = 100;

    /**
     * 特效播放结束
     */
    public static final int MSG_STAT_EFFECTS_END = 200;

    /**
     * 消息分类为错误
     */
    public static int MSG_TYPE_ERROR = 6;

    public static final int MSG_ERROR_FILE_NOT_EXIST = 300;

    public static final int MSG_ERROR_STREAM_NOT_FOUND = 400;

    public static final int MSG_ERROR_VIDEO_STREAM_NOT_FOUND = 500;

    public static final int MSG_ERROR_AVCODE_OPEN_FAILED = 600;


}
