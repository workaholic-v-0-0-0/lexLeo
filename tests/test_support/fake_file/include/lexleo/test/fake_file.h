/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_file.h
 * @ingroup test_support_fake_file
 * @brief Fake file backend for unit tests.
 *
 * @details
 * This header declares fake OSAL file operations and helper functions for
 * configuring and observing fake file behavior in tests.
 *
 * Tests can prepare the next fake `open()` result, configure fake file handles,
 * control operation statuses, inspect buffered and sink data, and observe
 * the arguments and call counts of file operations.
 */

#ifndef LEXLEO_FAKE_FILE_H
#define LEXLEO_FAKE_FILE_H

#include "osal/file/osal_file_ops.h"

#include "policy/lexleo_cstd_types.h"

#define FAKE_FILE_MAX_SEQ_LEN 256
#define FAKE_FILE_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fake_file_t fake_file_t;

static inline fake_file_t *
osal_file_to_fake_file(OSAL_FILE *file) { return (fake_file_t *)file; }

static inline OSAL_FILE *
fake_file_to_osal_file(fake_file_t *fake) { return (OSAL_FILE *)fake; }

typedef struct fake_file_ctrl_t
{
	/* cfg */
	osal_file_status_t next_open_status;
	fake_file_t *next_open_out;
	osal_file_status_t next_mkdir_status;

	/* spy */
	size_t open_call_count;
	OSAL_FILE **last_open_out;
	const char *last_open_pathname;
	const char *last_open_mode;
	const osal_mem_ops_t *last_open_mem_ops;
	size_t mkdir_call_count;
	const char *last_mkdir_pathname;

} fake_file_ctrl_t;

extern fake_file_ctrl_t g_fake_file_ctrl_seq[FAKE_FILE_MAX_SEQ_LEN];

typedef struct fake_file_t
{
	/* state */
	bool is_open;
	const char *pathname;
	const char *mode;
	uint8_t buffered_backing[FAKE_FILE_BUF_SIZE];
	uint8_t sink_backing[FAKE_FILE_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t pos;

	/* cfg */
	osal_file_status_t next_read_status;
	osal_file_status_t next_write_status;
	osal_file_status_t next_flush_status;
	osal_file_status_t next_close_status;
	osal_file_status_t next_gets_status;

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

// CFG

void fake_file_reset(void);
void fake_file_init_instance(fake_file_t *fake);

void fake_file_set_buffered_backing(
	fake_file_t *fake,
	const uint8_t *data,
	size_t len);
void fake_file_set_sink_backing(
	fake_file_t *fake,
	const uint8_t *data,
	size_t len);
void fake_file_set_pos(
	fake_file_t *fake,
	size_t n);

// FAKE API

osal_file_status_t fake_file_open(
	OSAL_FILE **out,
	const char *pathname,
	const char *mode,
	const osal_mem_ops_t *mem_ops);

size_t fake_file_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st);

size_t fake_file_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st);

osal_file_status_t fake_file_flush(OSAL_FILE *stream);

osal_file_status_t fake_file_close(OSAL_FILE *stream);

char *fake_file_gets(
	char *out,
	size_t out_size,
	OSAL_FILE *stream,
	osal_file_status_t *st);

osal_file_status_t fake_file_mkdir(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_FILE_H */
