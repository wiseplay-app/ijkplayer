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

#ifndef IJKREADER__FF_READER_H
#define IJKREADER__FF_READER_H

#include <libavformat/avformat.h>
#include <ijksdl/ijksdl.h>

#define IJK_OPT_CATEGORY_FORMAT 1

typedef struct IjkReaderContext {
	AVDictionary *format_opts;
	AVIOContext *io_ctx;
} IjkReaderContext;

void ijkreader_init(IjkReaderContext **pr);
int  ijkreader_set_data_source(IjkReaderContext **pr, const char *path);
void ijkreader_set_option(IjkReaderContext **pr, int opt_category, const char *name, const char *value);
void ijkreader_set_option_int(IjkReaderContext **pr, int opt_category, const char *name, int64_t value);
int  ijkreader_read(IjkReaderContext **pr, uint8_t *buf, int size);
void ijkreader_release(IjkReaderContext **pr);

#endif
