#ifndef IJKCONV__JNI_H
#define IJKCONV__JNI_H

#include <jni.h>

#define LOGTAG "IJKCONV"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

JNIEXPORT jboolean JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_await(JNIEnv *, jobject, jint);
JNIEXPORT jint JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_run(JNIEnv *, jobject, jobjectArray);
JNIEXPORT void JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_stop(JNIEnv *, jobject, jint);

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // IJKCONV__JNI_H
