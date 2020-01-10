package com.ffmpeg.bbeffect;

/**
 * @auther SuZhanFeng
 * @date 2020/1/10
 * @desc
 */
public interface EffectPlayListener {
    /**
     *  播放动效回调
     * @param type  消息类型  分为：信息  错误
     * @param ret   消息码
     */
    void onAnimEvent(int type, int ret);
}
