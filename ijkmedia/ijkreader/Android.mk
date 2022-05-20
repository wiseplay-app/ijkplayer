LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_LDLIBS += -llog -landroid

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../ijkj4a)
LOCAL_C_INCLUDES += $(MY_APP_FFMPEG_INCLUDE_PATH)

LOCAL_SRC_FILES += ff_reader.c
LOCAL_SRC_FILES += ijkreader_jni.c

LOCAL_SHARED_LIBRARIES := ijkffmpeg ijksdl

LOCAL_MODULE := ijkreader
include $(BUILD_SHARED_LIBRARY)
