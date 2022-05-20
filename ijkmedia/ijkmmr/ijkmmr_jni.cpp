/*
 * ijkmmr_jni.cpp
 *      based on FFmpegMediaMetadataRetriever/wseemann_media_MediaMetadataRetriever.cpp
 *
 * Copyright (c) 2014 William Seemann
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "IjkMediaMetadataRetrieverJNI"

#include <assert.h>
#include <utils/Log.h>
#include <utils/Mutex.h>
#include <mediametadataretriever.h>

#include "jni.h"


using namespace std;

struct fields_t {
    jfieldID context;
};

typedef int32_t status_t;

static fields_t fields;
static Mutex sLock;
static const char* const kClassPathName = "tv/danmaku/ijk/media/player/IjkMediaMetadataRetriever";

void jniThrowException(JNIEnv *env, const char *className,
    const char *msg) {
    jclass exception = env->FindClass(className);
    env->ThrowNew(exception, msg);
}

static int jniGetFDFromFileDescriptor(JNIEnv *env, jobject fileDescriptor) {
    jint fd = -1;
    jclass fdClass = env->FindClass("java/io/FileDescriptor");
    
    if (fdClass != NULL) {
        jfieldID fdClassDescriptorFieldID = env->GetFieldID(fdClass, "descriptor", "I");
        if (fdClassDescriptorFieldID != NULL && fileDescriptor != NULL) {
            fd = env->GetIntField(fileDescriptor, fdClassDescriptorFieldID);
        }
    }
    
    return fd;
}

static void process_media_retriever_call(JNIEnv *env, status_t opStatus, const char* exception, const char *message)
{
    if (opStatus == (status_t) INVALID_OPERATION) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else if (opStatus != (status_t) OK) {
        if (strlen(message) > 230) {
            // If the message is too long, don't bother displaying the status code.
            jniThrowException( env, exception, message);
        } else {
            char msg[256];
            // Append the status code to the message.
            sprintf(msg, "%s: status = 0x%X", message, opStatus);
            jniThrowException( env, exception, msg);
        }
    }
}

static MediaMetadataRetriever* getRetriever(JNIEnv* env, jobject thiz)
{
    // No lock is needed, since it is called internally by other methods that are protected
    MediaMetadataRetriever* retriever = (MediaMetadataRetriever*) env->GetLongField(thiz, fields.context);
    return retriever;
}

static MediaMetadataRetriever* setRetriever(JNIEnv* env, jobject thiz, MediaMetadataRetriever* retriever)
{
    // No lock is needed, since it is called internally by other methods that are protected
    MediaMetadataRetriever *old = (MediaMetadataRetriever*) env->GetLongField(thiz, fields.context);
    env->SetLongField(thiz, fields.context, (jlong) retriever);
    return old;
}

static void
IjkMediaMetadataRetriever_setDataSourceAndHeaders(
        JNIEnv *env, jobject thiz, jstring path,
        jobjectArray keys, jobjectArray values) {

    ALOGV("setDataSource");
    MediaMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(
                env,
                "java/lang/IllegalStateException", "No retriever available");

        return;
    }

    if (!path) {
        jniThrowException(
                env, "java/lang/IllegalArgumentException", "Null pointer");

        return;
    }

    const char *tmp = env->GetStringUTFChars(path, NULL);
    if (!tmp) {  // OutOfMemoryError exception already thrown
        return;
    }

    // Don't let somebody trick us in to reading some random block of memory
    if (strncmp("mem://", tmp, 6) == 0) {
        jniThrowException(
                env, "java/lang/IllegalArgumentException", "Invalid pathname");
        return;
    }

    char *headers = NULL;
    
    if (keys && values) {
        int keysCount = env->GetArrayLength(keys);
        int valuesCount = env->GetArrayLength(values);
        
        if (keysCount != valuesCount) {
            ALOGE("Keys and values arrays have different length");
            jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
            return;
        }
        
        int i = 0;
        const char *rawString = NULL;
        char hdrs[2048];
        
        for (i = 0; i < keysCount; i++) {
            jstring key = (jstring) env->GetObjectArrayElement(keys, i);
            rawString = env->GetStringUTFChars(key, NULL);
            strcat(hdrs, rawString);
            strcat(hdrs, ": ");
            env->ReleaseStringUTFChars(key, rawString);
            
            jstring value = (jstring) env->GetObjectArrayElement(values, i);
            rawString = env->GetStringUTFChars(value, NULL);
            strcat(hdrs, rawString);
            strcat(hdrs, "\r\n");
            env->ReleaseStringUTFChars(value, rawString);
        }
        
        headers = &hdrs[0];
    }
	
    process_media_retriever_call(
            env,
            retriever->setDataSource(tmp, headers),
            "java/lang/RuntimeException",
            "setDataSource failed");		
    env->ReleaseStringUTFChars(path, tmp);
}

static void IjkMediaMetadataRetriever_setDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
    ALOGV("setDataSource");
    MediaMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return;
    }
    if (!fileDescriptor) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    if (offset < 0 || length < 0 || fd < 0) {
        if (offset < 0) {
            ALOGE("negative offset (%lld)", (long long)offset);
        }
        if (length < 0) {
            ALOGE("negative length (%lld)", (long long)length);
        }
        if (fd < 0) {
            ALOGE("invalid file descriptor");
        }
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    process_media_retriever_call(env, retriever->setDataSource(fd, offset, length), "java/lang/RuntimeException", "setDataSource failed");
}

template<typename T>
static void rotate0(T* dst, const T* src, size_t width, size_t height)
{
    memcpy(dst, src, width * height * sizeof(T));
}

template<typename T>
static void rotate90(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[j * height + height - 1 - i] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate180(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate270(T* dst, const T* src, size_t width, size_t height)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            dst[(width - 1 - j) * height + i] = src[i * width + j];
        }
    }
}

template<typename T>
static void rotate(T *dst, const T *src, size_t width, size_t height, int angle)
{
    switch (angle) {
        case 0:
            rotate0(dst, src, width, height);
            break;
        case 90:
            rotate90(dst, src, width, height);
            break;
        case 180:
            rotate180(dst, src, width, height);
            break;
        case 270:
            rotate270(dst, src, width, height);
            break;
    }
}

static jbyteArray IjkMediaMetadataRetriever_getFrameAtTime(JNIEnv *env, jobject thiz, jlong timeUs, jint option)
{
    ALOGV("getFrameAtTime: %lld us option: %d", timeUs, option);
    MediaMetadataRetriever *retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }

    AVPacket packet;	
    av_init_packet(&packet);
	
    if (retriever->getFrameAtTime(timeUs, option, &packet) != 0) {
        ALOGE("getFrameAtTime: Call to getFrameAtTime failed.");
        av_packet_unref(&packet);
        return NULL;
    }
	
    jbyteArray array = env->NewByteArray(packet.size);
    if (!array) {  // OutOfMemoryError exception has already been thrown.
        ALOGE("getFrameAtTime: OutOfMemoryError is thrown.");
    } else {
        jbyte* bytes = env->GetByteArrayElements(array, NULL);
        if (bytes != NULL) {
            memcpy(bytes, (uint8_t *) packet.data, packet.size);
            env->ReleaseByteArrayElements(array, bytes, 0);
        }
    }
	
    av_packet_unref(&packet);
   
    return array;
}

static jbyteArray IjkMediaMetadataRetriever_getEmbeddedPicture(
        JNIEnv *env, jobject thiz)
{
    ALOGV("getEmbeddedPicture");
    MediaMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }

    AVPacket packet;
    if (retriever->extractAlbumArt(&packet) != 0) {
        ALOGE("getEmbeddedPicture: Call to getEmbeddedPicture failed.");
        return NULL;
    }
	
    jbyteArray array = env->NewByteArray(packet.size);
    if (!array) {  // OutOfMemoryError exception has already been thrown.
        ALOGE("getEmbeddedPicture: OutOfMemoryError is thrown.");
    } else {
        env->SetByteArrayRegion(array, 0, packet.size, (jbyte *) packet.data);
    }
	
    av_packet_unref(&packet);

    return array;
}

static jobject IjkMediaMetadataRetriever_extractMetadata(JNIEnv *env, jobject thiz, jstring jkey)
{
    ALOGV("extractMetadata");
    MediaMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    const char *key = env->GetStringUTFChars(jkey, NULL);
    if (!key) {
        return NULL;
    }
    const char* value = retriever->extractMetadata(key);
    if (!value) {
        ALOGV("extractMetadata: Metadata is not found");
        return NULL;
    }
    ALOGV("extractMetadata: value (%s) for keyCode(%s)", value, key);
    env->ReleaseStringUTFChars(jkey, key);
    return env->NewStringUTF(value);
}

static void IjkMediaMetadataRetriever_release(JNIEnv *env, jobject thiz)
{
    ALOGV("release");
    Mutex::Autolock lock(sLock);
    MediaMetadataRetriever* retriever = getRetriever(env, thiz);
    delete retriever;
    setRetriever(env, thiz, (MediaMetadataRetriever*) 0);
}

static void IjkMediaMetadataRetriever_native_finalize(JNIEnv *env, jobject thiz)
{
    ALOGV("native_finalize");
    // No lock is needed, since IjkMediaMetadataRetriever_release() is protected
    IjkMediaMetadataRetriever_release(env, thiz);
}

// This function gets a field ID, which in turn causes class initialization.
// It is called from a static block in MediaMetadataRetriever, which won't run until the
// first time an instance of this class is used.
static void IjkMediaMetadataRetriever_native_init(JNIEnv *env)
{
    jclass clazz = env->FindClass(kClassPathName);
    if (clazz == NULL) {
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (fields.context == NULL) {
        return;
    }
	
    // Initialize libavformat and register all the muxers, demuxers and protocols.
    av_register_all();
    avformat_network_init();
}

static void IjkMediaMetadataRetriever_native_setup(JNIEnv *env, jobject thiz)
{
    ALOGV("native_setup");
    MediaMetadataRetriever* retriever = new MediaMetadataRetriever();
    if (retriever == 0) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }
    setRetriever(env, thiz, retriever);
}

// JNI mapping between Java methods and native methods
static JNINativeMethod nativeMethods[] = {
        {
            "_setDataSource",
            "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
            (void *)IjkMediaMetadataRetriever_setDataSourceAndHeaders
        },

        {"setDataSource",   "(Ljava/io/FileDescriptor;JJ)V", (void *)IjkMediaMetadataRetriever_setDataSourceFD},
        {"_getFrameAtTime", "(JI)[B", (void *)IjkMediaMetadataRetriever_getFrameAtTime},
        {"extractMetadata", "(Ljava/lang/String;)Ljava/lang/String;", (void *)IjkMediaMetadataRetriever_extractMetadata},
        {"getEmbeddedPicture", "()[B", (void *)IjkMediaMetadataRetriever_getEmbeddedPicture},
        {"release",         "()V", (void *)IjkMediaMetadataRetriever_release},
        {"native_finalize", "()V", (void *)IjkMediaMetadataRetriever_native_finalize},
        {"native_setup",    "()V", (void *)IjkMediaMetadataRetriever_native_setup},
        {"native_init",     "()V", (void *)IjkMediaMetadataRetriever_native_init},
};

int register_IjkMediaMetadataRetriever(JNIEnv *env)
{
    int numMethods = (sizeof(nativeMethods) / sizeof(*nativeMethods));
    jclass clazz = env->FindClass(kClassPathName);
    jint ret = env->RegisterNatives(clazz, nativeMethods, numMethods);
    env->DeleteLocalRef(clazz);
    return ret;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    jint result = -1;
    
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);
    
    if (register_IjkMediaMetadataRetriever(env) < 0) {
        ALOGE("ERROR: IjkMediaMetadataRetriever native registration failed\n");
        goto bail;
    }
    
    result = JNI_VERSION_1_6;
    
bail:
    return result;
}
