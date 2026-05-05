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

#define FAKE_FILE_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

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

// CFG

OSAL_FILE *fake_file_create_fake(const osal_mem_ops_t *mem_ops);
void fake_file_reset(OSAL_FILE *fake);
void fake_file_prepare_next_open_file(OSAL_FILE *fake);
void fake_file_prepare_next_open_status(osal_file_status_t st);
void fake_file_prepare_next_mkdir_status(osal_file_status_t st);

void fake_file_set_buffered_backing(
	OSAL_FILE *fake,
	const uint8_t *data,
	size_t len);
void fake_file_set_sink_backing(
	OSAL_FILE *fake,
	const uint8_t *data,
	size_t len);
void fake_file_set_read_status(OSAL_FILE *fake, osal_file_status_t st);
void fake_file_set_write_status(OSAL_FILE *fake, osal_file_status_t st);
void fake_file_set_flush_status(OSAL_FILE *fake, osal_file_status_t st);
void fake_file_set_close_status(OSAL_FILE *fake, osal_file_status_t st);
void fake_file_set_gets_status(OSAL_FILE *fake, osal_file_status_t st);
void fake_file_set_pos(OSAL_FILE *fake, size_t n);

// SPY

size_t fake_file_open_call_count(void);
OSAL_FILE **fake_file_last_open_out(void);
const char *fake_file_last_open_pathname(void);
const char *fake_file_last_open_mode(void);
const osal_mem_ops_t *fake_file_last_open_mem_ops(void);

size_t fake_file_read_call_count(OSAL_FILE *fake);
void *fake_file_last_read_ptr(OSAL_FILE *fake);
size_t fake_file_last_read_size(OSAL_FILE *fake);
size_t fake_file_last_read_nmemb(OSAL_FILE *fake);
OSAL_FILE *fake_file_last_read_stream(OSAL_FILE *fake);
osal_file_status_t *fake_file_last_read_status_ptr(OSAL_FILE *fake);

size_t fake_file_write_call_count(OSAL_FILE *fake);
const void *fake_file_last_write_ptr(OSAL_FILE *fake);
size_t fake_file_last_write_size(OSAL_FILE *fake);
size_t fake_file_last_write_nmemb(OSAL_FILE *fake);
OSAL_FILE *fake_file_last_write_stream(OSAL_FILE *fake);
osal_file_status_t *fake_file_last_write_status_ptr(OSAL_FILE *fake);

size_t fake_file_flush_call_count(OSAL_FILE *fake);
OSAL_FILE *fake_file_last_flush_stream(OSAL_FILE *fake);

size_t fake_file_close_call_count(OSAL_FILE *fake);
OSAL_FILE *fake_file_last_close_stream(OSAL_FILE *fake);

size_t fake_file_gets_call_count(OSAL_FILE *fake);
char *fake_file_last_gets_out(OSAL_FILE *fake);
size_t fake_file_last_gets_out_size(OSAL_FILE *fake);
OSAL_FILE *fake_file_last_gets_stream(OSAL_FILE *fake);
osal_file_status_t *fake_file_last_gets_status_ptr(OSAL_FILE *fake);

size_t fake_file_mkdir_call_count(void);
const char *fake_file_last_mkdir_pathname(void);

const uint8_t *fake_file_buffered_backing(OSAL_FILE *fake);
const uint8_t *fake_file_sink_backing(OSAL_FILE *fake);
size_t fake_file_buffered_len(OSAL_FILE *fake);
size_t fake_file_sink_len(OSAL_FILE *fake);
size_t fake_file_pos(OSAL_FILE *fake);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_FILE_H */
