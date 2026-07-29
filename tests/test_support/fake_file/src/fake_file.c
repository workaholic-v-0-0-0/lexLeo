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

fake_file_ctrl_t g_fake_file_ctrl_seq[FAKE_FILE_MAX_SEQ_LEN] = {0};
static size_t g_fake_file_ctrl_seq_next = 0;

void fake_file_reset(void)
{
	osal_memset(
		g_fake_file_ctrl_seq,
		0,
		sizeof g_fake_file_ctrl_seq
	);
	g_fake_file_ctrl_seq_next = 0;
	for (size_t i = 0; i < FAKE_FILE_MAX_SEQ_LEN; i++) {
		g_fake_file_ctrl_seq[i].next_open_status = OSAL_FILE_STATUS_OK;
		g_fake_file_ctrl_seq[i].next_mkdir_status = OSAL_FILE_STATUS_OK;
	}
}

void fake_file_init_instance(fake_file_t *fake_file)
{
	LEXLEO_ASSERT(fake_file);

	osal_memset(fake_file, 0, sizeof(*fake_file));

	fake_file->next_read_status = OSAL_FILE_STATUS_OK;
	fake_file->next_write_status = OSAL_FILE_STATUS_OK;
	fake_file->next_flush_status = OSAL_FILE_STATUS_OK;
	fake_file->next_close_status = OSAL_FILE_STATUS_OK;
	fake_file->next_gets_status = OSAL_FILE_STATUS_OK;
}

void fake_file_set_buffered_backing(
	fake_file_t *fake_file,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   fake_file
		&& (data || len == 0)
		&& len <= FAKE_FILE_BUF_SIZE
	);
	osal_memset(
		fake_file->buffered_backing,
		0,
		sizeof(fake_file->buffered_backing)
	);
	fake_file->pos = 0;
	fake_file->buffered_len = len;
	if (len > 0) {
		osal_memcpy(fake_file->buffered_backing, data, len);
	}
}

void fake_file_set_sink_backing(
	fake_file_t *fake_file,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   fake_file
		&& (data || len == 0)
		&& len <= FAKE_FILE_BUF_SIZE
	);

	osal_memset(
		fake_file->sink_backing,
		0,
		sizeof(fake_file->sink_backing)
	);
	fake_file->sink_len = len;
	if (len > 0) {
		osal_memcpy(fake_file->sink_backing, data, len);
	}
}

void fake_file_set_pos(
	fake_file_t *fake_file,
	size_t n
) {
	LEXLEO_ASSERT(fake_file && n <= fake_file->buffered_len);
	fake_file->pos = n;
}

osal_file_status_t fake_file_open(
	OSAL_FILE **out,
	const char *pathname,
	const char *mode,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(
		   out
		&& mode
		&& g_fake_file_ctrl_seq_next < FAKE_FILE_MAX_SEQ_LEN
	);

	fake_file_ctrl_t *ctrl =
		&g_fake_file_ctrl_seq[g_fake_file_ctrl_seq_next++];

	ctrl->open_call_count++;
	ctrl->last_open_out = out;
	ctrl->last_open_pathname = pathname;
	ctrl->last_open_mode = mode;
	ctrl->last_open_mem_ops = mem_ops;

	if (ctrl->next_open_status != OSAL_FILE_STATUS_OK) {
		return ctrl->next_open_status;
	}

	fake_file_t *fake_file = ctrl->next_open_out;

	LEXLEO_ASSERT(
		   fake_file
		&& fake_file->sink_len <= FAKE_FILE_BUF_SIZE
	);

	fake_file_set_buffered_backing(fake_file, NULL, 0);

	if (osal_strcmp(mode, "rb") == 0) {
		osal_memcpy(
			fake_file->buffered_backing,
			fake_file->sink_backing,
			fake_file->sink_len
		);
		fake_file->buffered_len = fake_file->sink_len;
		fake_file->pos = 0;
	} else if (osal_strcmp(mode, "wb") == 0) {
		fake_file_set_sink_backing(fake_file, NULL, 0);
		fake_file->pos = 0;
	} else if (osal_strcmp(mode, "ab") == 0) {
		osal_memcpy(
			fake_file->buffered_backing,
			fake_file->sink_backing,
			fake_file->sink_len
		);
		fake_file->buffered_len = fake_file->sink_len;
		fake_file->pos = fake_file->buffered_len;
	} else {
		LEXLEO_ASSERT(false);
	}

	fake_file->is_open = true;
	fake_file->pathname = pathname;
	fake_file->mode = mode;

	*out = fake_file_to_osal_file(fake_file);
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

	fake_file_t *fake_file = osal_file_to_fake_file(stream);

	fake_file->read_call_count++;
	fake_file->last_read_ptr = ptr;
	fake_file->last_read_size = size;
	fake_file->last_read_nmemb = nmemb;
	fake_file->last_read_stream = stream;
	fake_file->last_read_st = st;

	LEXLEO_ASSERT(
		   fake_file->buffered_len <= FAKE_FILE_BUF_SIZE
		&& fake_file->pos <= fake_file->buffered_len
	);

	if (
		   fake_file->next_read_status != OSAL_FILE_STATUS_OK
		&& fake_file->next_read_status != OSAL_FILE_STATUS_EOF
	) {
		*st = fake_file->next_read_status;
		return 0;
	}

	if (size == 0 || nmemb == 0) {
		*st = OSAL_FILE_STATUS_OK;
		return 0;
	}

	size_t requested_bytes = size * nmemb;
	size_t available_bytes = fake_file->buffered_len - fake_file->pos;
	size_t readable_bytes =
		(requested_bytes < available_bytes)
			? requested_bytes
			: available_bytes;

	size_t readable_nmemb = readable_bytes / size;
	size_t copied_bytes = readable_nmemb * size;

	if (copied_bytes > 0) {
		osal_memcpy(
			ptr,
			fake_file->buffered_backing + fake_file->pos,
			copied_bytes
		);
		fake_file->pos += copied_bytes;
	}

	if (readable_nmemb < nmemb) {
		*st = OSAL_FILE_STATUS_EOF;
	} else {
		*st = OSAL_FILE_STATUS_OK;
	}

	return readable_nmemb;
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

	fake_file_t *fake_file = osal_file_to_fake_file(stream);

	fake_file->write_call_count++;
	fake_file->last_write_ptr = ptr;
	fake_file->last_write_size = size;
	fake_file->last_write_nmemb = nmemb;
	fake_file->last_write_stream = stream;
	fake_file->last_write_st = st;

	*st = fake_file->next_write_status;

	if (fake_file->next_write_status != OSAL_FILE_STATUS_OK) {
		return 0;
	}

	if (size == 0 || nmemb == 0) {
		return 0;
	}

	size_t requested_bytes = size * nmemb;

	LEXLEO_ASSERT(fake_file->pos + requested_bytes <= FAKE_FILE_BUF_SIZE);

	if (requested_bytes) {
		osal_memcpy(
			fake_file->buffered_backing + fake_file->pos,
			ptr,
			requested_bytes
		);
		fake_file->pos += requested_bytes;
		if (fake_file->pos > fake_file->buffered_len) {
			fake_file->buffered_len = fake_file->pos;
		}
	}

	return nmemb;
}

osal_file_status_t fake_file_flush(
	OSAL_FILE *stream
) {
	LEXLEO_ASSERT(stream);

	fake_file_t *fake_file = osal_file_to_fake_file(stream);

	fake_file->flush_call_count++;
	fake_file->last_flush_stream = stream;

	if (fake_file->next_flush_status == OSAL_FILE_STATUS_OK) {
		LEXLEO_ASSERT(fake_file->buffered_len <= FAKE_FILE_BUF_SIZE);
		fake_file_set_sink_backing(fake_file, NULL, 0);
		if (fake_file->buffered_len != 0) {
			osal_memcpy(
				fake_file->sink_backing,
				fake_file->buffered_backing,
				fake_file->buffered_len
			);
		}
		fake_file->sink_len = fake_file->buffered_len;
	}

	return fake_file->next_flush_status;
}

osal_file_status_t fake_file_close(
	OSAL_FILE *stream
) {
	LEXLEO_ASSERT(stream);

	fake_file_t *fake_file = osal_file_to_fake_file(stream);

	fake_file->close_call_count++;
	fake_file->last_close_stream = stream;

	if (fake_file->next_close_status != OSAL_FILE_STATUS_OK) {
		return fake_file->next_close_status;
	}

	LEXLEO_ASSERT(fake_file_flush(stream) == OSAL_FILE_STATUS_OK);

	fake_file_set_buffered_backing(fake_file, NULL, 0);

	fake_file->is_open = false;

	return OSAL_FILE_STATUS_OK;
}

char *fake_file_gets(
	char *out,
	size_t out_size,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	LEXLEO_ASSERT(out && stream && st);

	fake_file_t *fake_file = osal_file_to_fake_file(stream);

	fake_file->gets_call_count++;
	fake_file->last_gets_out = out;
	fake_file->last_gets_out_size = out_size;
	fake_file->last_gets_stream = stream;
	fake_file->last_gets_st = st;

	*st = fake_file->next_gets_status;

	if (fake_file->next_gets_status != OSAL_FILE_STATUS_OK) {
		return NULL;
	}

	LEXLEO_ASSERT(
		   fake_file->pos <= fake_file->buffered_len
		&& fake_file->buffered_len <= FAKE_FILE_BUF_SIZE
		&& out_size != 0
	);
	size_t available_bytes = fake_file->buffered_len - fake_file->pos;

	if (available_bytes == 0) {
		return NULL;
	}

	char *ret = out;

	const uint8_t *p = fake_file->buffered_backing + fake_file->pos;
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
	fake_file->pos += len;

	return ret;
}

osal_file_status_t fake_file_mkdir(
	const char *pathname
) {
	LEXLEO_ASSERT(
		g_fake_file_ctrl_seq_next < FAKE_FILE_MAX_SEQ_LEN
	);
	fake_file_ctrl_t *ctrl =
		&g_fake_file_ctrl_seq[g_fake_file_ctrl_seq_next++];
	ctrl->mkdir_call_count++;
	ctrl->last_mkdir_pathname = pathname;
	return ctrl->next_mkdir_status;
}
