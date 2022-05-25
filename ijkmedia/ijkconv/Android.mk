# copyright (c) 2014-2022 3U Digital Business Ltd
#
# This file is part of ijkPlayer.
#
# ijkPlayer is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# ijkPlayer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with ijkPlayer; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -w
LOCAL_LDLIBS += -llog -landroid

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)

ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_CFLAGS += -marm
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-armv5)
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfloat-abi=soft
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-armv7a)
endif
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-arm64)
endif
ifeq ($(TARGET_ARCH_ABI),mips)
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-mips)
endif
ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-x86)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-x86_64)
endif

LOCAL_SRC_FILES += cmdutils.c ffmpeg.c ffmpeg_filter.c ffmpeg_hw.c ffmpeg_opt.c
LOCAL_SRC_FILES += ijkconv_jni.c

LOCAL_SHARED_LIBRARIES := ijkffmpeg

LOCAL_MODULE := ijkconv
include $(BUILD_SHARED_LIBRARY)
