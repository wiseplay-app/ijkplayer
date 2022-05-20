#include "ffmpeg.h"
#include "ijkconv_jni.h"

#include <stdlib.h>
#include <stdbool.h>
#include <android/log.h> 

extern int main(int argc, char **argv);

JavaVM *sVm = NULL;

static void ijkconv_log(void *ptr, int level, const char *fmt, va_list vl)
{
    int ffplv __unused = ANDROID_LOG_VERBOSE;
    if (level <= AV_LOG_ERROR)
        ffplv = ANDROID_LOG_ERROR;
    else if (level <= AV_LOG_WARNING)
        ffplv = ANDROID_LOG_WARN;
    else if (level <= AV_LOG_INFO)
        ffplv = ANDROID_LOG_INFO;
    else if (level <= AV_LOG_VERBOSE)
        ffplv = ANDROID_LOG_VERBOSE;
    else
        ffplv = ANDROID_LOG_DEBUG;

    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

    __android_log_print(ffplv, LOGTAG, "%s", line);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    sVm = vm;
    return JNI_VERSION_1_6;
}

JNIEXPORT jboolean JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_await(JNIEnv *env, jobject obj, jint pid) {
    int status = 1;
    waitpid(pid, &status, 0);
    return (status == 0);
}

JNIEXPORT jint JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_run(JNIEnv *env, jobject obj, jobjectArray args) {
    int i = 0;
    int argc = 0;
    char **argv = NULL;
    jstring *strr = NULL;
	
    int pid = fork();	
    if (pid != 0) {
        return pid;
    }

    if (args != NULL) {
        argc = (*env)->GetArrayLength(env, args);
        argv = (char **) malloc(sizeof(char *) * argc);
        strr = (jstring *) malloc(sizeof(jstring) * argc);

        for (i = 0; i < argc; ++i) {
            strr[i] = (jstring)(*env)->GetObjectArrayElement(env, args, i);
            argv[i] = (char *)(*env)->GetStringUTFChars(env, strr[i], 0);
        }
    }	
		
    av_log_set_callback(ijkconv_log);
    int result = main(argc, argv);

    for (i = 0; i < argc; ++i) {
        (*env)->ReleaseStringUTFChars(env, strr[i], argv[i]);
    }
    free(argv);
    free(strr);

    _exit(result);
    return 0;
}

JNIEXPORT void JNICALL Java_tv_danmaku_ijk_media_player_IjkConv_stop(JNIEnv *env, jobject obj, jint pid) {
    kill(pid, SIGTERM);
}
