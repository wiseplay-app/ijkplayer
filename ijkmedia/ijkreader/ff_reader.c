/*
 * Copyright (C) 2014-2022 3U Digital Business Ltd
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

#include <stdlib.h>
#include <unistd.h>

#include "ff_reader.h"

#define LOG_TAG "IJKREADER"

static void ijktranscoder_log(void *ptr, int level, const char *fmt, va_list vl)
{
	int ffplv __unused = IJK_LOG_VERBOSE;
    if (level <= AV_LOG_ERROR)
        ffplv = IJK_LOG_ERROR;
    else if (level <= AV_LOG_WARNING)
        ffplv = IJK_LOG_WARN;
    else if (level <= AV_LOG_INFO)
        ffplv = IJK_LOG_INFO;
    else if (level <= AV_LOG_VERBOSE)
        ffplv = IJK_LOG_VERBOSE;
    else
        ffplv = IJK_LOG_DEBUG;

    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

    ALOG(ffplv, LOG_TAG, "%s", line);
}

static AVDictionary **ijkreader_get_opt_dict(IjkReaderContext *ctx, int opt_category)
{
	switch (opt_category) {
	case IJK_OPT_CATEGORY_FORMAT:
		return &ctx->format_opts;
	default:
		ALOGE("unknown option category %d", opt_category);
		return NULL;
	}
}

static void ijkreader_close(IjkReaderContext *ctx)
{
	ALOGV("Closing and freeing resources.");
	
	if (ctx->io_ctx) {
		avio_close(ctx->io_ctx);
	}
	ctx->io_ctx = NULL;
}

static void ijkreader_show_dict(const char *tag, AVDictionary *dict)
{
    AVDictionaryEntry *t = NULL;

    while ((t = av_dict_get(dict, "", t, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_INFO, "%-*s: %-*s = %s\n", 12, tag, 28, t->key, t->value);
    }
}

static int ijkreader_open_input(IjkReaderContext *ctx, const char *path)
{
	AVIOContext *io_ctx = NULL;
	int rc = 0;
	
	ALOGV("Opening the input: %s", path);
	
	ijkreader_show_dict("format-opts", ctx->format_opts);

	rc = avio_open(&io_ctx, path, AVIO_FLAG_READ);
	if (rc < 0) {
		ALOGE("Failed to open the input.");
		goto fail;
	}	
	ctx->io_ctx = io_ctx;
	
	ALOGV("Input opened successfully.");

fail:
	return rc;
}

static void ijkreader_reset(IjkReaderContext *ctx)
{
	ALOGV("Resetting reader context.");

	ijkreader_close(ctx);
}

//-----------------------------------------------

void ijkreader_init(IjkReaderContext **pr)
{
	IjkReaderContext *ctx = *pr;
	
	if (!ctx) {
		ctx = (IjkReaderContext *)av_mallocz(sizeof(IjkReaderContext));
	}
	*pr = ctx;
#if 1
	av_log_set_callback(ijktranscoder_log);
#endif
}

int ijkreader_set_data_source(IjkReaderContext **pr, const char *path)
{
	IjkReaderContext *ctx = *pr;
	int rc = 0;
	
	ijkreader_reset(ctx);
	
	rc = ijkreader_open_input(ctx, path);
	if (rc < 0)
		ijkreader_reset(ctx);
	
	return rc;
}

void ijkreader_set_option(IjkReaderContext **pr, int opt_category, const char *name, const char *value)
{
	IjkReaderContext *ctx = *pr;
	if (!ctx)
		return;

	AVDictionary **dict = ijkreader_get_opt_dict(ctx, opt_category);
	if (dict)
		av_dict_set(dict, name, value, 0);
}

void ijkreader_set_option_int(IjkReaderContext **pr, int opt_category, const char *name, int64_t value)
{
	IjkReaderContext *ctx = *pr;
	if (!ctx)
		return;

	AVDictionary **dict = ijkreader_get_opt_dict(ctx, opt_category);
	if (dict)
		av_dict_set_int(dict, name, value, 0);
}

int ijkreader_read(IjkReaderContext **pr, uint8_t *buf, int size)
{
	IjkReaderContext *ctx = *pr;
	
	ALOGV("Reading %d bytes from pipe...", size);
	int ret = avio_read(ctx->io_ctx, buf, size);
	ALOGV("Read %d bytes.", ret);
	
	return ret;
}

void ijkreader_release(IjkReaderContext **pr)
{
	IjkReaderContext *ctx = *pr;

	if (ctx) {
		if (ctx->format_opts) {
			av_dict_free(&ctx->format_opts);
		}
		
		ijkreader_reset(ctx);		
		av_free(ctx);
	}	
	*pr = NULL;
}
