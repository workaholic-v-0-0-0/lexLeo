/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_FAKE_STDIO_H
#define LEXLEO_FAKE_STDIO_H

#include "osal/stdio/osal_stdio_ops.h"

#include "policy/lexleo_cstd_types.h"

#define FAKE_STDIO_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

OSAL_STDIO *fake_stdio_stdin(void);
OSAL_STDIO *fake_stdio_stdout(void);
OSAL_STDIO *fake_stdio_stderr(void);

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

// CFG

void fake_stdio_reset(void);

void fake_stdio_set_buffered_backing(
	OSAL_STDIO *stdio,
	const uint8_t *backing,
	size_t len);
void fake_stdio_set_sink_backing(
	OSAL_STDIO *stdio,
	const uint8_t *backing,
	size_t len);
void fake_stdio_set_read_pos(OSAL_STDIO *stdio, size_t n);

// SPY

size_t fake_stdio_read_call_count(OSAL_STDIO *stdio);
size_t fake_stdio_write_call_count(OSAL_STDIO *stdio);
size_t fake_stdio_flush_call_count(OSAL_STDIO *stdio);
const uint8_t *fake_stdio_buffered_backing(OSAL_STDIO *stdio);
const uint8_t *fake_stdio_sink_backing(OSAL_STDIO *stdio);
size_t fake_stdio_buffered_len(OSAL_STDIO *stdio);
size_t fake_stdio_sink_len(OSAL_STDIO *stdio);
size_t fake_stdio_read_pos(OSAL_STDIO *stdio);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_FAKE_STDIO_H
