/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stdio.h
 * @ingroup test_support_fake_stdio
 * @brief Fake stdio backend for unit tests.
 *
 * @details
 * This header declares fake standard streams and stdio-like operations for
 * tests.
 *
 * Tests can configure stream contents and operation results, inspect buffered
 * and flushed data, reset fake state, and observe read, write, flush, error,
 * and EOF calls.
 */

#ifndef LEXLEO_FAKE_STDIO_H
#define LEXLEO_FAKE_STDIO_H

#include "osal/stdio/osal_stdio_ops.h"

#include "policy/lexleo_cstd_types.h"

#define FAKE_STDIO_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fake_stdio_t
{
	/* state */

	uint8_t buffered_backing[FAKE_STDIO_BUF_SIZE];
	uint8_t sink_backing[FAKE_STDIO_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t read_pos;
	bool end_of_input_has_been_encountered;

	/* cfg */

	int next_error_ret;
	int next_flush_ret;
	bool end_of_input_has_been_signaled;

	/* spy */

	size_t read_call_count;
	void *last_read_ptr;
	size_t last_read_size;
	size_t last_read_nmemb;
	OSAL_STDIO *last_read_stdio;

	size_t write_call_count;
	const void *last_write_ptr;
	size_t last_write_size;
	size_t last_write_nmemb;
	OSAL_STDIO *last_write_stdio;

	size_t flush_call_count;
	OSAL_STDIO *last_flush_stdio;

	size_t error_call_count;
	OSAL_STDIO *last_error_stdio;

	size_t eof_call_count;
	OSAL_STDIO *last_eof_stdio;

} fake_stdio_t;

static inline fake_stdio_t *osal_stdio_to_fake_stdio(OSAL_STDIO *stdio) { return (fake_stdio_t *)stdio; }
static inline OSAL_STDIO *fake_stdio_to_osal_stdio(fake_stdio_t *fake) { return (OSAL_STDIO *)fake; }

typedef struct fake_stdio_ctrl_t
{
	/* state */
	fake_stdio_t stdin;
	fake_stdio_t stdout;
	fake_stdio_t stderr;

	/* spy */
	size_t get_stdin_call_count;
	size_t get_stdout_call_count;
	size_t get_stderr_call_count;

} fake_stdio_ctrl_t;

extern fake_stdio_ctrl_t g_fake_stdio_ctrl;

/* CFG */

void fake_stdio_reset(void);
void fake_stdio_init_instance(fake_stdio_t *fake_stdio);

void fake_stdio_set_buffered_backing(
	fake_stdio_t *fake_stdio,
	const uint8_t *backing,
	size_t len);
void fake_stdio_set_sink_backing(
	fake_stdio_t *fake_stdio,
	const uint8_t *backing,
	size_t len);
void fake_stdio_set_read_pos(
	fake_stdio_t *fake_stdio,
	size_t n);

/* FAKE API */

OSAL_STDIO *fake_get_stdio_stdin(void);
OSAL_STDIO *fake_get_stdio_stdout(void);
OSAL_STDIO *fake_get_stdio_stderr(void);

size_t fake_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio);

size_t fake_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio);

int fake_stdio_flush(OSAL_STDIO *stdio);
int fake_stdio_error(OSAL_STDIO *stdio);
int fake_stdio_eof(OSAL_STDIO *stdio);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_STDIO_H */
