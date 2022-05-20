/*
 * mediametadataretriever.cpp
 *      based on FFmpegMediaMetadataRetriever/mediametadataretriever.cpp
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

//#define LOG_NDEBUG 0
#define LOG_TAG "MediaMetadataRetriever"

#include <mediametadataretriever.h>

using namespace std;

MediaMetadataRetriever::MediaMetadataRetriever()
{
	state = NULL;
}

MediaMetadataRetriever::~MediaMetadataRetriever()
{
	::ijkmmr_release(&state);
}

int MediaMetadataRetriever::setDataSource(const char *srcUrl, const char *headers)
{
	return ::ijkmmr_set_data_source_uri(&state, srcUrl, headers);
}

int MediaMetadataRetriever::setDataSource(int fd, int64_t offset, int64_t length)
{
    return ::ijkmmr_set_data_source_fd(&state, fd, offset, length);
}

int MediaMetadataRetriever::getFrameAtTime(int64_t timeUs, int option, AVPacket *pkt)
{
    return ::ijkmmr_get_frame_at_time(&state, timeUs, option, pkt);
}

const char* MediaMetadataRetriever::extractMetadata(const char *key)
{
    return ::ijkmmr_extract_metadata(&state, key);
}

int MediaMetadataRetriever::extractAlbumArt(AVPacket *pkt)
{
    return ::ijkmmr_get_embedded_picture(&state, pkt);
}
