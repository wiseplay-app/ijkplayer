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
