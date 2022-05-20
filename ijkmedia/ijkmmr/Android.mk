LOCAL_PATH += $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -DHAVE_PTHREADS -Wno-deprecated -Wno-deprecated-declarations
LOCAL_LDLIBS += -llog

LOCAL_C_INCLUDES += $(MY_APP_FFMPEG_INCLUDE_PATH)

LOCAL_SRC_FILES += ijkmmr.c
LOCAL_SRC_FILES += mediametadataretriever.cpp
LOCAL_SRC_FILES += ijkmmr_jni.cpp

LOCAL_SHARED_LIBRARIES := libijkffmpeg

LOCAL_MODULE := libijkmmr
include $(BUILD_SHARED_LIBRARY)
