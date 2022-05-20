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
