/*
 * Copyright (C) 2014-2022 3U Digital Business Ltd
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ff_reader.h"
#include "jni.h"
#include "ijksdl/android/ijksdl_android_jni.h"

#define JNI_CLASS_NAME "tv/danmaku/ijk/media/player/IjkReader"

struct fields_t {
    jfieldID context;
};

static struct fields_t fields;

static void
process_transcoder_call(JNIEnv *env, int opStatus, const char *exception, const char *message)
{
    if (opStatus != 0) {
        if (strlen(message) > 230) {
            SDL_JNI_ThrowException(env, exception, message);
        } else {
            char msg[256];
            sprintf(msg, "%s: status = 0x%X", message, opStatus);
            SDL_JNI_ThrowException(env, exception, msg);
        }
    }
}

//-----------------------------------------------

static IjkReaderContext *getReaderContext(JNIEnv* env, jobject thiz)
{
    IjkReaderContext *ctx = (IjkReaderContext *) (intptr_t) (*env)->GetLongField(env, thiz, fields.context);
    return ctx;
}

static IjkReaderContext *setReaderContext(JNIEnv* env, jobject thiz, IjkReaderContext* ctx)
{
    IjkReaderContext *old = (IjkReaderContext *) (intptr_t) (*env)->GetLongField(env, thiz, fields.context);
    (*env)->SetLongField(env, thiz, fields.context, (intptr_t)ctx);
	return old;
}

static void
IjkReader_setDataSource(JNIEnv *env, jobject thiz, jstring path)
{
    ALOGV("setDataSource");
    IjkReaderContext *ctx = getReaderContext(env, thiz);
	
    if (!ctx) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalStateException", "No context available");
        return;
    }

    if (!path) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalArgumentException", "Null pointer");
        return;
    }

    const char *cpath = (*env)->GetStringUTFChars(env, path, NULL);
    if (!cpath) {
        return;
    }
	
    process_transcoder_call(
            env,
            ijkreader_set_data_source(&ctx, cpath),
            "java/lang/RuntimeException",
            "setDataSource failed");	
			
	(*env)->ReleaseStringUTFChars(env, path, cpath);
}

static void
IjkReader_setOption(JNIEnv *env, jobject thiz, jint category, jobject name, jobject value)
{
    ALOGV("setOption");
    IjkReaderContext *ctx = getReaderContext(env, thiz);
	const char *c_name = NULL;
    const char *c_value = NULL;
	
	if (!ctx) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalStateException", "No context available");
        return;
    }
	
    c_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (value) {
		c_value = (*env)->GetStringUTFChars(env, value, NULL);
	}

    ijkreader_set_option(&ctx, category, c_name, c_value);

	if (c_name)
		(*env)->ReleaseStringUTFChars(env, name, c_name);
    if (c_value)
        (*env)->ReleaseStringUTFChars(env, value, c_value);
}

static void
IjkReader_setOptionLong(JNIEnv *env, jobject thiz, jint category, jobject name, jlong value)
{
    ALOGV("setOptionLong");
    IjkReaderContext *ctx = getReaderContext(env, thiz);
	const char *c_name = NULL;

	if (!ctx) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalStateException", "No context available");
        return;
    }

    c_name = (*env)->GetStringUTFChars(env, name, NULL);

    ijkreader_set_option_int(&ctx, category, c_name, value);

	if (c_name)
		(*env)->ReleaseStringUTFChars(env, name, c_name);
}

static jint
IjkReader_read(JNIEnv *env, jobject thiz, jbyteArray buffer, jint size)
{
    ALOGV("read");
	IjkReaderContext *ctx = getReaderContext(env, thiz);

    if (!ctx) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalStateException", "No context available");
        return -1;
    }
	
    if (!buffer || size <= 0) {
        SDL_JNI_ThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return -1;
    }
	
    jbyte *buf = (*env)->GetByteArrayElements(env, buffer, NULL);
	int read = ijkreader_read(&ctx, (uint8_t *)buf, size);
	
	(*env)->ReleaseByteArrayElements(env, buffer, buf, JNI_ABORT);
	
	return read;
}

static void IjkReader_release(JNIEnv *env, jobject thiz)
{
    ALOGV("release");
	IjkReaderContext *ctx = getReaderContext(env, thiz);
	
    ijkreader_release(&ctx);
    setReaderContext(env, thiz, (IjkReaderContext *)0);
}

static void IjkReader_native_finalize(JNIEnv *env, jobject thiz)
{
    ALOGV("native_finalize");
    IjkReader_release(env, thiz);
}

static void IjkReader_native_init(JNIEnv *env)
{
    jclass clazz = (*env)->FindClass(env, JNI_CLASS_NAME);
    if (clazz == NULL) {
        return;
    }

    fields.context = (*env)->GetFieldID(env, clazz, "mNativeContext", "J");
    if (fields.context == NULL) {
        return;
    }
	
	avformat_network_init();
}

static void IjkReader_native_setup(JNIEnv *env, jobject thiz)
{
    ALOGV("native_setup");
	IjkReaderContext *ctx = NULL;
	
	ijkreader_init(&ctx);
    if (!ctx) {
        SDL_JNI_ThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }
    setReaderContext(env, thiz, ctx);
}

//-----------------------------------------------

static JNINativeMethod nativeMethods[] = {
    { "_setOption",       "(ILjava/lang/String;Ljava/lang/String;)V", (void *) IjkReader_setOption },
    { "_setOption",       "(ILjava/lang/String;J)V",                  (void *) IjkReader_setOptionLong },
    {"setDataSource",     "(Ljava/lang/String;)V", (void *) IjkReader_setDataSource},
    {"read",              "([BI)I", (void *) IjkReader_read},
    {"release",           "()V", (void *) IjkReader_release},
    {"native_finalize",   "()V", (void *) IjkReader_native_finalize},
    {"native_setup",      "()V", (void *) IjkReader_native_setup},
    {"native_init",       "()V", (void *) IjkReader_native_init},
};

int register_IjkReader(JNIEnv *env)
{
    int numMethods = (sizeof(nativeMethods) / sizeof(*nativeMethods));
    jclass clazz = (*env)->FindClass(env, JNI_CLASS_NAME);
    jint ret = (*env)->RegisterNatives(env, clazz, nativeMethods, numMethods);
    (*env)->DeleteLocalRef(env, clazz);
    return ret;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    jint result = -1;
    
    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);
    
    if (register_IjkReader(env) < 0) {
        ALOGE("ERROR: IjkReader native registration failed\n");
        goto bail;
    }
    
    result = JNI_VERSION_1_6;
    
bail:
    return result;
}
