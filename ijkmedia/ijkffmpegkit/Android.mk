LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -w -DFFMPEG_KIT_BUILD_DATE=$(shell date +%Y%m%d) -DFFMPEG_KIT_VERSION=4.4.0
LOCAL_LDLIBS += -llog -landroid

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfloat-abi=soft -DFFMPEG_KIT_ARM_V7A
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-armv7a)
endif
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_CFLAGS += -mfloat-abi=soft -DFFMPEG_KIT_ARM64_V8A
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-arm64)
endif
ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -mfloat-abi=soft -DFFMPEG_KIT_X86
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-x86)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
LOCAL_CFLAGS += -mfloat-abi=soft -DFFMPEG_KIT_X86_64
LOCAL_C_INCLUDES += $(realpath $(MY_APP_ANDROID_ROOT)/contrib/ffmpeg-x86_64)
endif

LOCAL_SRC_FILES += libavutil_file.c ffmpegkit.c ffprobekit.c ffmpegkit_exception.c fftools_cmdutils.c fftools_ffmpeg.c fftools_ffprobe.c fftools_ffmpeg_opt.c fftools_ffmpeg_hw.c fftools_ffmpeg_filter.c
LOCAL_SRC_FILES += android_lts_support.c

LOCAL_SHARED_LIBRARIES := ijkffmpeg

LOCAL_MODULE := ijkffmpegkit
include $(BUILD_SHARED_LIBRARY)
