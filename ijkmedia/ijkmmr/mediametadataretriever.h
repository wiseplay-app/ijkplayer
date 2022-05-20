/*
 * mediametadataretriever.h
 *      based on FFmpegMediaMetadataRetriever/mediametadataretriever.h
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

#ifndef IJKMMR__MEDIAMETADATARETRIEVER_H
#define IJKMMR__MEDIAMETADATARETRIEVER_H

extern "C" {
    #include "ijkmmr.h"
}

class MediaMetadataRetriever
{
	IjkMmrState *state;
	
public:
    MediaMetadataRetriever();
    ~MediaMetadataRetriever();
	
    int setDataSource(const char *dataSourceUrl, const char *headers);
    int setDataSource(int fd, int64_t offset, int64_t length);
    int getFrameAtTime(int64_t timeUs, int option, AVPacket *pkt);
    int extractAlbumArt(AVPacket *pkt);
    const char *extractMetadata(const char *key);
};

#endif
