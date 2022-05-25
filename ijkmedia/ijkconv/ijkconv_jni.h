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
