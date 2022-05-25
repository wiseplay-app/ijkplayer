/*
 * ijkmmr.h
 *      based on FFmpegMediaMetadataRetriever/ffmpeg_mediametadataretriever.h
 *
 * Copyright (c) 2014 William Seemann
 * Copyright (c) 2014-2022 3U Digital Business Ltd
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

#ifndef IJKMMR__FF_MMR_H
#define IJKMMR__FF_MMR_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef enum IjkMmrOptions {
	OPTION_PREVIOUS_SYNC = 0,
	OPTION_NEXT_SYNC = 1,
	OPTION_CLOSEST_SYNC = 2,
	OPTION_CLOSEST = 3,
	// Add more here...
} IjkMmrOptions;

typedef struct IjkMmrState {
	AVFormatContext *format_ctx;
	int             audio_stream;
	int             video_stream;
	AVStream        *audio_st;
	AVStream        *video_st;
	int             fd;
	int64_t         offset;
	const char      *headers;
} IjkMmrState;


int         ijkmmr_set_data_source_uri(IjkMmrState **ps, const char *path, const char *headers);
int         ijkmmr_set_data_source_fd(IjkMmrState **ps, int fd, int64_t offset, int64_t length);
const char *ijkmmr_extract_metadata(IjkMmrState **ps, const char *key);
int         ijkmmr_get_embedded_picture(IjkMmrState **ps, AVPacket *pkt);
int         ijkmmr_get_frame_at_time(IjkMmrState **ps, int64_t timeUs, int option, AVPacket *pkt);
void        ijkmmr_release(IjkMmrState **ps);


#endif

