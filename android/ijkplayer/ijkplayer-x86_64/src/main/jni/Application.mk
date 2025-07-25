# Copyright (c) 2013-2014 Bilibili
# copyright (c) 2013-2014 Zhang Rui <bbcallen@gmail.com>
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

APP_OPTIM := release
APP_PLATFORM := android-21
APP_ABI := x86_64
APP_PIE := false

APP_STL := c++_static

APP_CFLAGS := -O3 -Wall -pipe \
    -ffast-math \
    -U_FORTIFY_SOURCE  -D_FORTIFY_SOURCE=0 \
    -fstrict-aliasing -Werror=strict-aliasing \
    -Wa,--noexecstack \
    -DANDROID -DNDEBUG

LOCAL_LDFLAGS += -Wl,-z,max-page-size=16384