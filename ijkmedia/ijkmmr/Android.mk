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
