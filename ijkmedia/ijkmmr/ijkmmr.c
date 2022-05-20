/*
 * ijkmmr.c
 *      based on FFmpegMediaMetadataRetriever/ffmpeg_mediametadataretriever.c
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

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <ijkmmr.h>
#include <stdio.h>
#include <unistd.h>

#if 0
# include <utils/Log.h>

# define LOG_TAG "IjkMediaMetadataRetriever"
# define printf(fmt, args...) ALOGE(fmt, ##args)
#endif

const int TARGET_IMAGE_CODEC = AV_CODEC_ID_PNG;
const int TARGET_IMAGE_FORMAT = AV_PIX_FMT_RGB24;

const char *AUDIO_CODEC = "audio_codec";
const char *DURATION = "duration";
const char *FRAMERATE = "framerate";
const char *ICY_METADATA = "icy_metadata";
const char *ROTATE = "rotate";
const char *VIDEO_CODEC = "video_codec";

const int FAILURE = -1;
const int SUCCESS = 0;

int is_supported_format(int codec_id)
{
	if (codec_id == AV_CODEC_ID_PNG ||
            codec_id == AV_CODEC_ID_MJPEG ||
	    codec_id == AV_CODEC_ID_BMP) {
		return 1;
	}

	return 0;
}

void convert_image(AVCodecContext *codec_ctx, AVFrame *in_frame, AVPacket *avpkt, int *got_packet_ptr)
{
	AVCodecContext *enc_ctx = NULL;
	AVCodec *codec;
	AVFrame *frame = NULL;
	struct SwsContext *scaler_ctx = NULL;
	int ret = -1;

	*got_packet_ptr = 0;

	codec = avcodec_find_encoder(TARGET_IMAGE_CODEC);
	if (!codec) {
		printf("avcodec_find_encoder() failed to find encoder\n");
		goto fail;
	}

	enc_ctx = avcodec_alloc_context3(codec);
	if (!enc_ctx) {
		printf("avcodec_alloc_context3 failed\n");
		goto fail;
	}

	enc_ctx->bit_rate = codec_ctx->bit_rate;
	enc_ctx->width = codec_ctx->width;
	enc_ctx->height = codec_ctx->height;
	enc_ctx->pix_fmt = TARGET_IMAGE_FORMAT;
	enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	enc_ctx->time_base.num = codec_ctx->time_base.num;
	enc_ctx->time_base.den = codec_ctx->time_base.den;

	if (!codec || avcodec_open2(enc_ctx, codec, NULL) < 0) {
	  	printf("avcodec_open2() failed (encoder)\n");
		goto fail;
	}

	frame = av_frame_alloc();
	if (!frame) {
		goto fail;
	}

	frame->width = codec_ctx->width;
	frame->height = codec_ctx->height;
	frame->format = TARGET_IMAGE_FORMAT;

	int numBytes = avpicture_get_size(TARGET_IMAGE_FORMAT, codec_ctx->width, codec_ctx->height);
	void *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

	avpicture_fill(((AVPicture *)frame),
			buffer,
			TARGET_IMAGE_FORMAT,
			enc_ctx->width,
			enc_ctx->height);

	scaler_ctx = sws_getContext(codec_ctx->width,
			codec_ctx->height,
			codec_ctx->pix_fmt,
			codec_ctx->width,
			codec_ctx->height,
			TARGET_IMAGE_FORMAT,
			SWS_FAST_BILINEAR, 0, 0, 0);

	if (!scaler_ctx) {
		printf("sws_getContext() failed\n");
		goto fail;
	}

	sws_scale(scaler_ctx,
			(const uint8_t * const *) in_frame->data,
			in_frame->linesize,
			0,
			in_frame->height,
			frame->data,
			frame->linesize);

	ret = avcodec_encode_video2(enc_ctx, avpkt, frame, got_packet_ptr);
	if (ret < 0) {
		*got_packet_ptr = 0;
	}

	av_free(buffer);

fail:
	if (frame) {
		av_free(frame);
	}

	if (enc_ctx) {
		avcodec_close(enc_ctx);
		av_free(enc_ctx);
	}

	if (scaler_ctx) {
		sws_freeContext(scaler_ctx);
	}

	if (ret < 0 || !(*got_packet_ptr)) {
		av_packet_unref(avpkt);
	}
}

void decode_frame(IjkMmrState *state, AVPacket *pkt, int *got_frame, int64_t desired_frame_number)
{
	AVFrame *frame = av_frame_alloc();

	*got_frame = 0;

	if (!frame) {
		return;
	}

	// Read frames and return the first one found
	while (av_read_frame(state->format_ctx, pkt) >= 0) {
		// Is this a packet from the video stream?
		if (pkt->stream_index == state->video_stream) {
			int codec_id = state->video_st->codec->codec_id;

			// If the image isn't already in a supported format convert it to one
			if (!is_supported_format(codec_id)) {
				*got_frame = 0;

				// Decode video frame
				if (avcodec_decode_video2(state->video_st->codec, frame, got_frame, pkt) <= 0) {
					*got_frame = 0;
					break;
				}

				// Did we get a video frame?
				if (*got_frame) {
					if (desired_frame_number == -1 ||
					   (desired_frame_number != -1 && frame->pkt_pts >= desired_frame_number)) {
						av_init_packet(pkt);
						pkt->data = NULL;
	  					pkt->size = 0;

						convert_image(state->video_st->codec, frame, pkt, got_frame);
						break;
					}
				}
			} else {
				*got_frame = 1;
				break;
			}
		}
		av_packet_unref(pkt);
	}

	av_frame_free(&frame);
}

size_t first_char_pos(const char *value, const char ch)
{
	char *chptr = strchr(value, ch);
	return chptr - value;
}

size_t last_char_pos(const char *value, const char ch)
{
	char *chptr = strrchr(value, ch);
	return chptr - value;
}

void get_shoutcast_metadata(AVFormatContext *ic)
{
	char *value = NULL;

	if (av_opt_get(ic, "icy_metadata_packet", 1, (uint8_t **) &value) < 0) {
		value = NULL;
	}

	if (value && value[0]) {
		av_dict_set(&ic->metadata, ICY_METADATA, value, 0);
	}
}

void get_duration(AVFormatContext *ic, char * value)
{
	int duration = 0;

	if (ic) {
		if (ic->duration != AV_NOPTS_VALUE) {
			duration = ((ic->duration / AV_TIME_BASE) * 1000);
		}
	}

	sprintf(value, "%d", duration); // %i
}

void set_codec(AVFormatContext *ic, int i)
{
	const char *codec_type = av_get_media_type_string(ic->streams[i]->codec->codec_type);

	if (!codec_type) {
		return;
	}

	const char *codec_name = avcodec_get_name(ic->streams[i]->codec->codec_id);

	if (strcmp(codec_type, "audio") == 0) {
		av_dict_set(&ic->metadata, AUDIO_CODEC, codec_name, 0);
	} else if (codec_type && strcmp(codec_type, "video") == 0) {
	   	av_dict_set(&ic->metadata, VIDEO_CODEC, codec_name, 0);
	}
}

void set_rotation(IjkMmrState *s)
{
	if (!ijkmmr_extract_metadata(&s, ROTATE) && s->video_st && s->video_st->metadata) {
		AVDictionaryEntry *entry = av_dict_get(s->video_st->metadata, ROTATE, NULL, AV_DICT_IGNORE_SUFFIX);

		if (entry && entry->value) {
			av_dict_set(&s->format_ctx->metadata, ROTATE, entry->value, 0);
		}
	}
}

void set_framerate(IjkMmrState *s)
{
	char value[30] = "0";

	if (s->video_st && s->video_st->avg_frame_rate.den && s->video_st->avg_frame_rate.num) {
		double d = av_q2d(s->video_st->avg_frame_rate);
		uint64_t v = lrintf(d * 100);

		if (v % 100) {
			sprintf(value, "%3.2f", d);
		} else if (v % (100 * 1000)) {
			sprintf(value,  "%1.0f", d);
		} else {
			sprintf(value, "%1.0fk", d / 1000);
		}

		av_dict_set(&s->format_ctx->metadata, FRAMERATE, value, 0);
	}
}

int stream_component_open(IjkMmrState *s, int stream_index)
{
	AVFormatContext *format_ctx = s->format_ctx;
	AVCodecContext *codec_ctx;
	AVCodec *codec;

	if (stream_index < 0 || stream_index >= format_ctx->nb_streams) {
		return FAILURE;
	}

	// Get a pointer to the codec context for the stream
	codec_ctx = format_ctx->streams[stream_index]->codec;

	// Find the decoder for the audio stream
	codec = avcodec_find_decoder(codec_ctx->codec_id);

	if(codec == NULL) {
		printf("avcodec_find_decoder() failed to find audio decoder\n");
		return FAILURE;
	}

	// Open the codec
	if (!codec || (avcodec_open2(codec_ctx, codec, NULL) < 0)) {
	  	printf("avcodec_open2() failed (decoder)\n");
		return FAILURE;
	}

	switch(codec_ctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			s->audio_stream = stream_index;
			s->audio_st = format_ctx->streams[stream_index];
			break;
		case AVMEDIA_TYPE_VIDEO:
			s->video_stream = stream_index;
			s->video_st = format_ctx->streams[stream_index];
			break;
		default:
			break;
	}

	return SUCCESS;
}

int set_data_source_l(IjkMmrState **ps, const char* path)
{
	int audio_index = -1;
	int video_index = -1;
	int i;

	IjkMmrState *state = *ps;
	char duration[30] = "0";

	AVDictionary *options = NULL;
	av_dict_set(&options, "icy", "1", 0);
	av_dict_set(&options, "user-agent", "IjkMediaMetadataRetriever", 0);

	if (state->headers) {
		av_dict_set(&options, "headers", state->headers, 0);
	}

	if (state->offset > 0) {
		state->format_ctx = avformat_alloc_context();
		state->format_ctx->skip_initial_bytes = state->offset;
	}

	if (avformat_open_input(&state->format_ctx, path, NULL, &options) != 0) {
		printf("Path could not be opened\n");
		*ps = NULL;
		return FAILURE;
	}

	if (avformat_find_stream_info(state->format_ctx, NULL) < 0) {
		printf("Metadata could not be retrieved\n");
		avformat_close_input(&state->format_ctx);
		*ps = NULL;
		return FAILURE;
	}

	get_duration(state->format_ctx, duration);
	av_dict_set(&state->format_ctx->metadata, DURATION, duration, 0);

	get_shoutcast_metadata(state->format_ctx);

	// Find the first audio and video stream
	for (i = 0; i < state->format_ctx->nb_streams; i++) {
		if (state->format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && video_index < 0) {
			video_index = i;
		}

		if (state->format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audio_index < 0) {
			audio_index = i;
		}

		set_codec(state->format_ctx, i);
	}

	if (audio_index >= 0) {
		stream_component_open(state, audio_index);
	}

	if (video_index >= 0) {
		stream_component_open(state, video_index);
	}

	set_rotation(state);
	set_framerate(state);

	*ps = state;
	return SUCCESS;
}

void reset_state(IjkMmrState **ps)
{
	IjkMmrState *state = *ps;

	if (state) {
		state->format_ctx = NULL;
		state->audio_stream = -1;
		state->video_stream = -1;
		state->audio_st = NULL;
		state->video_st = NULL;
		state->fd = -1;
		state->offset = 0;
		state->headers = NULL;
	}
}

void close_state(IjkMmrState **ps)
{
	IjkMmrState *state = *ps;

	if (state) {
		if (state->format_ctx) {
			int i;

			for (i = 0; i < state->format_ctx->nb_streams; i++) {
				AVCodecContext *codec_ctx = state->format_ctx->streams[i]->codec;
				if (codec_ctx) {
					avcodec_close(codec_ctx);
				}
			}
			avformat_close_input(&state->format_ctx);
		}

		if (state->fd != -1) {
			close(state->fd);
		}

		reset_state(&state);
	}
}

IjkMmrState* init_state(IjkMmrState **ps)
{
	IjkMmrState *state = *ps;

	if (!state) {
		state = av_mallocz(sizeof(IjkMmrState));
		*ps = state;
	} else {
		close_state(&state);
	}

	reset_state(&state);

	return state;
}

int ijkmmr_set_data_source_uri(IjkMmrState **ps, const char* path, const char* headers)
{
	IjkMmrState *state = init_state(ps);

	state->headers = headers;

	return set_data_source_l(ps, path);
}

int ijkmmr_set_data_source_fd(IjkMmrState **ps, int fd, int64_t offset, int64_t length)
{
	char path[256] = "", str[20];
	int myfd = dup(fd);

	sprintf(str, "pipe:%d", myfd);
	strcat(path, str);

	IjkMmrState *state = init_state(ps);

	state->fd = myfd;
	state->offset = offset;

	return set_data_source_l(ps, path);
}

const char* ijkmmr_extract_metadata(IjkMmrState **ps, const char* key)
{
	IjkMmrState *state = *ps;
	char* value = NULL;

	if (!state || !state->format_ctx) {
		return value;
	}

	if (key) {
		if (av_dict_get(state->format_ctx->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)) {
			value = av_dict_get(state->format_ctx->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)->value;
		} else if (state->audio_st && av_dict_get(state->audio_st->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)) {
			value = av_dict_get(state->audio_st->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)->value;
		} else if (state->video_st && av_dict_get(state->video_st->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)) {
			value = av_dict_get(state->video_st->metadata, key, NULL, AV_DICT_IGNORE_SUFFIX)->value;
		}
	}

	return value;
}

int ijkmmr_get_embedded_picture(IjkMmrState **ps, AVPacket *pkt)
{
	int i = 0;
	int got_packet = 0;
	AVFrame *frame = NULL;

	IjkMmrState *state = *ps;

	if (!state || !state->format_ctx) {
		return FAILURE;
	}

	// read the format headers
	if (state->format_ctx->iformat->read_header(state->format_ctx) < 0) {
		printf("Could not read the format header\n");
		return FAILURE;
	}

	// find the first attached picture, if available
	for (i = 0; i < state->format_ctx->nb_streams; i++) {
		if (state->format_ctx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
			*pkt = state->format_ctx->streams[i]->attached_pic;

			// Is this a packet from the video stream?
			if (pkt->stream_index == state->video_stream) {
				int codec_id = state->video_st->codec->codec_id;

				// If the image isn't already in a supported format convert it to one
				if (!is_supported_format(codec_id)) {
					int got_frame = 0;

					av_init_packet(pkt);

   					frame = av_frame_alloc();
   					if (!frame) {
   						break;
					}

					if (avcodec_decode_video2(state->video_st->codec, frame, &got_frame, pkt) <= 0) {
						break;
					}

					// Did we get a video frame?
					if (got_frame) {
						AVPacket packet;

						av_init_packet(&packet);
						packet.data = NULL;
						packet.size = 0;

						convert_image(state->video_st->codec, frame, &packet, &got_packet);

						*pkt = packet;
						break;
					}
				} else {
					av_init_packet(pkt);
					pkt->data = state->format_ctx->streams[i]->attached_pic.data;
					pkt->size = state->format_ctx->streams[i]->attached_pic.size;

					got_packet = 1;
					break;
				}
			}
		}
	}

	av_free(frame);

	if (got_packet) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

int ijkmmr_get_frame_at_time(IjkMmrState **ps, int64_t time_us, int option, AVPacket *pkt)
{
	int got_packet = 0;
	int64_t desired_frame_number = -1;

	IjkMmrState *state = *ps;
	IjkMmrOptions opt = option;

	if (!state || !state->format_ctx || state->video_stream < 0) {
		return FAILURE;
	}

	if (time_us != -1) {
		int stream_index = state->video_stream;
		int64_t seek_time = av_rescale_q(time_us, AV_TIME_BASE_Q, state->format_ctx->streams[stream_index]->time_base);
		int64_t seek_stream_duration = state->format_ctx->streams[stream_index]->duration;

		int flags = 0;
		int ret = -1;

		// For some reason the seek_stream_duration is sometimes a negative value,
		// make sure to check that it is greater than 0 before adjusting the
		// seek_time
		if (seek_stream_duration > 0 && seek_time > seek_stream_duration) {
			seek_time = seek_stream_duration;
		}

		if (seek_time < 0) {
			return FAILURE;
	   	}

		if (opt == OPTION_CLOSEST) {
			desired_frame_number = seek_time;
			flags = AVSEEK_FLAG_BACKWARD; 
		} else if (opt == OPTION_CLOSEST_SYNC) {
			flags = 0;
		} else if (opt == OPTION_NEXT_SYNC) {
			flags = 0;
		} else if (opt == OPTION_PREVIOUS_SYNC) {
			flags = AVSEEK_FLAG_BACKWARD;
		}

		ret = av_seek_frame(state->format_ctx, stream_index, seek_time, flags);

		if (ret < 0) {
			return FAILURE;
		} else {
			if (state->audio_stream >= 0) {
				avcodec_flush_buffers(state->audio_st->codec);
			}

			if (state->video_stream >= 0) {
				avcodec_flush_buffers(state->video_st->codec);
			}
		}
	}

	decode_frame(state, pkt, &got_packet, desired_frame_number);

	if (got_packet) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

void ijkmmr_release(IjkMmrState **ps)
{
	IjkMmrState *state = *ps;

	if (state) {
		close_state(&state);
		av_freep(&state);

		*ps = NULL;
	}
}
