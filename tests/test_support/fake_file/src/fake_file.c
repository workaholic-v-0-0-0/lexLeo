/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_file.c
 * @ingroup test_support_fake_file
 * @brief Fake file backend implementation for unit tests.
 *
 * @details
 * This file implements fake OSAL file operations and helper functions for
 * configuring and observing fake file behavior in tests.
 *
 * The fake backend supports configurable `open()` and `mkdir()` results,
 * prepared fake file handles, controllable read/write/flush/close/gets
 * statuses, buffered and sink data inspection, and file operation call spies.
 */

#include "lexleo/test/fake_file.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

typedef struct fake_file_t
{
	/* state */

	bool is_open;
	const char *pathname;
	const char *mode;
	const osal_mem_ops_t *mem_ops;
	uint8_t buffered_backing[FAKE_FILE_BUF_SIZE];
	uint8_t sink_backing[FAKE_FILE_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t pos;

	/* cfg */

	osal_file_status_t read_status;
	osal_file_status_t write_status;
	osal_file_status_t flush_status;
	osal_file_status_t close_status;
	osal_file_status_t gets_status;

	/* spy */

	size_t read_call_count;
	void *last_read_ptr;
	size_t last_read_size;
	size_t last_read_nmemb;
	OSAL_FILE *last_read_stream;
	osal_file_status_t *last_read_st;

	size_t write_call_count;
	const void *last_write_ptr;
	size_t last_write_size;
	size_t last_write_nmemb;
	OSAL_FILE *last_write_stream;
	osal_file_status_t *last_write_st;

	size_t flush_call_count;
	OSAL_FILE *last_flush_stream;

	size_t close_call_count;
	OSAL_FILE *last_close_stream;

	size_t gets_call_count;
	char *last_gets_out;
	size_t last_gets_out_size;
	OSAL_FILE *last_gets_stream;
	osal_file_status_t *last_gets_st;
} fake_file_t;

typedef struct fake_file_ctrl
{
	/* cfg */

	osal_file_status_t open_status;
	fake_file_t *open_out;

	osal_file_status_t mkdir_status;

	/* spy */

	size_t open_call_count;
	OSAL_FILE **last_open_out;
	const char *last_open_pathname;
	const char *last_open_mode;
	const osal_mem_ops_t *last_open_mem_ops;

	size_t mkdir_call_count;
	const char *last_mkdir_pathname;
} fake_file_ctrl;

static fake_file_ctrl g_fake_file_ctrl;

static fake_file_t *osal_file_to_fake_file(
	OSAL_FILE *file
) {
	return (fake_file_t *)file;
}

static OSAL_FILE *fake_file_to_osal_file(
	fake_file_t *fake
) {
	return (OSAL_FILE *)fake;
}

OSAL_FILE *fake_file_create_fake(
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(mem_ops && mem_ops->calloc);
	fake_file_t *fake = mem_ops->calloc(1, sizeof(*fake));
	if (!fake) {
		return NULL;
	}
	fake->mem_ops = mem_ops;
	return fake_file_to_osal_file(fake);
}

void fake_file_reset(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	const osal_mem_ops_t *mem_ops = casted_fake->mem_ops;
	osal_memset(casted_fake, 0, sizeof(*casted_fake));
	casted_fake->mem_ops = mem_ops;
	casted_fake->read_status = OSAL_FILE_STATUS_OK;
	casted_fake->write_status = OSAL_FILE_STATUS_OK;
	casted_fake->flush_status = OSAL_FILE_STATUS_OK;
	casted_fake->close_status = OSAL_FILE_STATUS_OK;
	casted_fake->gets_status = OSAL_FILE_STATUS_OK;
}

void fake_file_prepare_next_open_file(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	g_fake_file_ctrl.open_out = casted_fake;
}

void fake_file_prepare_next_open_status(
	osal_file_status_t st
) {
	g_fake_file_ctrl.open_status = st;
}

void fake_file_prepare_next_mkdir_status(
	osal_file_status_t st
) {
	g_fake_file_ctrl.mkdir_status = st;
}

void fake_file_set_buffered_backing(
	OSAL_FILE *fake,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   fake
		&& (data || len == 0)
		&& len <= FAKE_FILE_BUF_SIZE
	);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	osal_memset(casted_fake->buffered_backing, 0, FAKE_FILE_BUF_SIZE);
	casted_fake->pos = 0;
	casted_fake->buffered_len = len;
	if (len > 0) {
		osal_memcpy(casted_fake->buffered_backing, data, len);
	}
}

void fake_file_set_sink_backing(
	OSAL_FILE *fake,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   fake
		&& (data || len == 0)
		&& len <= FAKE_FILE_BUF_SIZE
	);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	osal_memset(casted_fake->sink_backing, 0, FAKE_FILE_BUF_SIZE);
	casted_fake->sink_len = len;
	if (len > 0) {
		osal_memcpy(casted_fake->sink_backing, data, len);
	}
}

void fake_file_set_read_status(
	OSAL_FILE *fake,
	osal_file_status_t st
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	casted_fake->read_status = st;
}

void fake_file_set_write_status(
	OSAL_FILE *fake,
	osal_file_status_t st
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	casted_fake->write_status = st;
}

void fake_file_set_flush_status(
	OSAL_FILE *fake,
	osal_file_status_t st
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	casted_fake->flush_status = st;
}

void fake_file_set_close_status(
	OSAL_FILE *fake,
	osal_file_status_t st
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	casted_fake->close_status = st;
}

void fake_file_set_gets_status(
	OSAL_FILE *fake,
	osal_file_status_t st
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	casted_fake->gets_status = st;
}

void fake_file_set_pos(
	OSAL_FILE *fake,
	size_t n
) {
	LEXLEO_ASSERT(fake);
	fake_file_t *casted_fake = osal_file_to_fake_file(fake);
	LEXLEO_ASSERT(n <= casted_fake->buffered_len);
	casted_fake->pos = n;
}

osal_file_status_t fake_file_open(
	OSAL_FILE **out,
	const char *pathname,
	const char *mode,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(out && mode);

	g_fake_file_ctrl.open_call_count++;
	g_fake_file_ctrl.last_open_out = out;
	g_fake_file_ctrl.last_open_pathname = pathname;
	g_fake_file_ctrl.last_open_mode = mode;
	g_fake_file_ctrl.last_open_mem_ops = mem_ops;

	if (g_fake_file_ctrl.open_status != OSAL_FILE_STATUS_OK) {
		return g_fake_file_ctrl.open_status;
	}

	fake_file_t *fake = g_fake_file_ctrl.open_out;
	LEXLEO_ASSERT(fake);

	LEXLEO_ASSERT(fake->sink_len <= FAKE_FILE_BUF_SIZE);

	OSAL_FILE *fake_osal_file = fake_file_to_osal_file(fake);
	fake_file_set_buffered_backing(fake_osal_file, NULL, 0);

	if (osal_strcmp(mode, "rb") == 0) {
		osal_memcpy(fake->buffered_backing, fake->sink_backing, fake->sink_len);
		fake->buffered_len = fake->sink_len;
		fake->pos = 0;
	} else if (osal_strcmp(mode, "wb") == 0) {
		fake_file_set_sink_backing(fake_osal_file, NULL, 0);
		fake->pos = 0;
	} else if (osal_strcmp(mode, "ab") == 0) {
		osal_memcpy(fake->buffered_backing, fake->sink_backing, fake->sink_len);
		fake->buffered_len = fake->sink_len;
		fake->pos = fake->buffered_len;
	} else {
		LEXLEO_ASSERT(false);
	}

	fake->is_open = true;
	fake->pathname = pathname;
	fake->mode = mode;

	*out = fake_osal_file;

	return OSAL_FILE_STATUS_OK;
}

size_t fake_file_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	LEXLEO_ASSERT(ptr && stream && st);

	size_t ret = 0;

	fake_file_t *fake = osal_file_to_fake_file(stream);

	fake->read_call_count++;
	fake->last_read_ptr = ptr;
	fake->last_read_size = size;
	fake->last_read_nmemb = nmemb;
	fake->last_read_stream = stream;
	fake->last_read_st = st;

	LEXLEO_ASSERT(
		   fake->buffered_len <= FAKE_FILE_BUF_SIZE
		&& fake->pos <= fake->buffered_len
	);

	if (size != 0 && fake->read_status == OSAL_FILE_STATUS_OK) {
		size_t requested_bytes = size * nmemb;
		size_t available_bytes = fake->buffered_len - fake->pos;
		size_t readable_bytes =
			(requested_bytes < available_bytes)
				? requested_bytes
				: available_bytes;
		size_t readable_nmemb = readable_bytes / size;
		ret = readable_nmemb * size;
		if (ret > 0) {
			osal_memcpy(
				ptr,
				fake->buffered_backing + fake->pos,
				ret
			);
			fake->pos += ret;
		}
	}

	*st = fake->read_status;

	return size ? ret / size : (size_t)0;
}

size_t fake_file_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	LEXLEO_ASSERT(ptr && stream && st);
	LEXLEO_ASSERT(size == 0 || nmemb <= FAKE_FILE_BUF_SIZE / size);

	fake_file_t *fake = osal_file_to_fake_file(stream);

	fake->write_call_count++;
	fake->last_write_ptr = ptr;
	fake->last_write_size = size;
	fake->last_write_nmemb = nmemb;
	fake->last_write_stream = stream;
	fake->last_write_st = st;

	*st = fake->write_status;

	if (fake->write_status != OSAL_FILE_STATUS_OK) {
		return 0;
	}

	if (size == 0 || nmemb == 0) {
		return 0;
	}

	size_t requested_bytes = size * nmemb;

	LEXLEO_ASSERT(fake->pos + requested_bytes <= FAKE_FILE_BUF_SIZE);

	if (requested_bytes) {
		osal_memcpy(
			fake->buffered_backing + fake->pos,
			ptr,
			requested_bytes
		);
		fake->pos += requested_bytes;
		if (fake->pos > fake->buffered_len) {
			fake->buffered_len = fake->pos;
		}
	}

	return nmemb;
}

osal_file_status_t fake_file_flush(
	OSAL_FILE *stream
) {
	LEXLEO_ASSERT(stream);

	fake_file_t *fake = osal_file_to_fake_file(stream);

	fake->flush_call_count++;
	fake->last_flush_stream = stream;

	if (fake->flush_status == OSAL_FILE_STATUS_OK) {
		LEXLEO_ASSERT(fake->buffered_len <= FAKE_FILE_BUF_SIZE);
		fake_file_set_sink_backing(stream, NULL, 0);
		if (fake->buffered_len != 0) {
			osal_memcpy(
				fake->sink_backing,
				fake->buffered_backing,
				fake->buffered_len
			);
		}
		fake->sink_len = fake->buffered_len;
	}

	return fake->flush_status;
}

osal_file_status_t fake_file_close(
	OSAL_FILE *stream
) {
	LEXLEO_ASSERT(stream);

	fake_file_t *fake = osal_file_to_fake_file(stream);

	fake->close_call_count++;
	fake->last_close_stream = stream;

	if (fake->close_status != OSAL_FILE_STATUS_OK) {
		return fake->close_status;
	}

	LEXLEO_ASSERT(fake_file_flush(stream) == OSAL_FILE_STATUS_OK);

	fake_file_set_buffered_backing(stream, NULL, 0);

	fake->is_open = false;

	return OSAL_FILE_STATUS_OK;
}

char *fake_file_gets(
	char *out,
	size_t out_size,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	LEXLEO_ASSERT(out && stream && st);

	fake_file_t *fake = osal_file_to_fake_file(stream);

	fake->gets_call_count++;
	fake->last_gets_out = out;
	fake->last_gets_out_size = out_size;
	fake->last_gets_stream = stream;
	fake->last_gets_st = st;

	*st = fake->gets_status;

	if (fake->gets_status != OSAL_FILE_STATUS_OK) {
		return NULL;
	}

	LEXLEO_ASSERT(
		   fake->pos <= fake->buffered_len
		&& fake->buffered_len <= FAKE_FILE_BUF_SIZE
		&& out_size != 0
	);
	size_t available_bytes = fake->buffered_len - fake->pos;

	if (available_bytes == 0) {
		return NULL;
	}

	char *ret = out;

	const uint8_t *p = fake->buffered_backing + fake->pos;
	size_t len = 0;
	while (
		   len < available_bytes
		&& len + 1 < out_size
		&& *p != '\n'
	) {
		len++;
		*out++ = (char)*p++;
	}
	if (
		   len < available_bytes
		&& *p == '\n'
		&& len + 1 < out_size
	) {
		len++;
		*out++ = '\n';
	}
 	*out = '\0';
	fake->pos += len;

	return ret;
}

osal_file_status_t fake_file_mkdir(
	const char *pathname
) {
	g_fake_file_ctrl.mkdir_call_count++;
	g_fake_file_ctrl.last_mkdir_pathname = pathname;

	return g_fake_file_ctrl.mkdir_status;
}

size_t fake_file_open_call_count(void)
{
	return g_fake_file_ctrl.open_call_count;
}

OSAL_FILE **fake_file_last_open_out(void)
{
	return g_fake_file_ctrl.last_open_out;
}

const char *fake_file_last_open_pathname(void)
{
	return g_fake_file_ctrl.last_open_pathname;
}

const char *fake_file_last_open_mode(void)
{
	return g_fake_file_ctrl.last_open_mode;
}

const osal_mem_ops_t *fake_file_last_open_mem_ops(void)
{
	return g_fake_file_ctrl.last_open_mem_ops;
}

size_t fake_file_read_call_count(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->read_call_count;
}

void *fake_file_last_read_ptr(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_read_ptr;
}

size_t fake_file_last_read_size(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_read_size;
}

size_t fake_file_last_read_nmemb(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_read_nmemb;
}

OSAL_FILE *fake_file_last_read_stream(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_read_stream;
}

osal_file_status_t *fake_file_last_read_status_ptr(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_read_st;
}

size_t fake_file_write_call_count(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->write_call_count;
}

const void *fake_file_last_write_ptr(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_write_ptr;
}

size_t fake_file_last_write_size(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_write_size;
}

size_t fake_file_last_write_nmemb(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_write_nmemb;
}

OSAL_FILE *fake_file_last_write_stream(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_write_stream;
}

osal_file_status_t *fake_file_last_write_status_ptr(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_write_st;
}

size_t fake_file_flush_call_count(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->flush_call_count;
}

OSAL_FILE *fake_file_last_flush_stream(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_flush_stream;
}

size_t fake_file_close_call_count(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->close_call_count;
}

OSAL_FILE *fake_file_last_close_stream(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_close_stream;
}

size_t fake_file_gets_call_count(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->gets_call_count;
}

char *fake_file_last_gets_out(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_gets_out;
}

size_t fake_file_last_gets_out_size(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_gets_out_size;
}

OSAL_FILE *fake_file_last_gets_stream(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_gets_stream;
}

osal_file_status_t *fake_file_last_gets_status_ptr(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->last_gets_st;
}

size_t fake_file_mkdir_call_count(void)
{
	return g_fake_file_ctrl.mkdir_call_count;
}

const char *fake_file_last_mkdir_pathname(void)
{
	return g_fake_file_ctrl.last_mkdir_pathname;
}

const uint8_t *fake_file_buffered_backing(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->buffered_backing;
}

const uint8_t *fake_file_sink_backing(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->sink_backing;
}

size_t fake_file_buffered_len(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->buffered_len;
}

size_t fake_file_sink_len(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->sink_len;
}

size_t fake_file_pos(
	OSAL_FILE *fake
) {
	LEXLEO_ASSERT(fake);
	return osal_file_to_fake_file(fake)->pos;
}
