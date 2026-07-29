/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stdio.c
 * @ingroup test_support_fake_stdio
 * @brief Fake stdio backend implementation for unit tests.
 *
 * @details
 * This file implements the fake standard streams, their in-memory state, and
 * the configuration and observation entry points declared in `fake_stdio.h`.
 */

#include "lexleo/test/fake_stdio.h"

#include "osal/mem/osal_mem.h"

#include "lexleo_cmocka.h"

size_t get_stdin_call_count = 0;
size_t get_stdout_call_count = 0;
size_t get_stderr_call_count = 0;

fake_stdio_t g_fake_stdio_stdin = {0};
fake_stdio_t g_fake_stdio_stdout = {0};
fake_stdio_t g_fake_stdio_stderr = {0};

static void fake_stdio_stream_reset(fake_stdio_t *fake_stdio)
{
	assert_non_null(fake_stdio);

	fake_stdio->read_call_count = 0;
	fake_stdio->write_call_count = 0;
	fake_stdio->flush_call_count = 0;

	osal_memset(fake_stdio->buffered_backing, 0, FAKE_STDIO_BUF_SIZE);
	osal_memset(fake_stdio->sink_backing, 0, FAKE_STDIO_BUF_SIZE);

	fake_stdio->buffered_len = 0;
	fake_stdio->sink_len = 0;
	fake_stdio->read_pos = 0;
}

void fake_stdio_reset(void)
{
	get_stdin_call_count = 0;
	get_stdout_call_count = 0;
	get_stderr_call_count = 0;
	fake_stdio_stream_reset(&g_fake_stdio_stdin);
	fake_stdio_stream_reset(&g_fake_stdio_stdout);
	fake_stdio_stream_reset(&g_fake_stdio_stderr);
}

void fake_stdio_set_buffered_backing(
	fake_stdio_t *fake_stdio,
	const uint8_t *backing,
	size_t len)
{
	assert_non_null(fake_stdio);
	assert_true(backing || len == 0);
	assert_true(len <= FAKE_STDIO_BUF_SIZE);

	if (len > 0) {
		osal_memcpy(fake_stdio->buffered_backing, backing, len);
	}
	if (len < FAKE_STDIO_BUF_SIZE) {
		osal_memset(fake_stdio->buffered_backing + len, 0, FAKE_STDIO_BUF_SIZE - len);
	}

	fake_stdio->buffered_len = len;
	fake_stdio->read_pos = 0;
}

void fake_stdio_set_sink_backing(
	fake_stdio_t *fake_stdio,
	const uint8_t *backing,
	size_t len)
{
	assert_non_null(fake_stdio);
	assert_true(backing || len == 0);
	assert_true(len <= FAKE_STDIO_BUF_SIZE);

	if (len > 0) {
		osal_memcpy(fake_stdio->sink_backing, backing, len);
	}
	if (len < FAKE_STDIO_BUF_SIZE) {
		osal_memset(fake_stdio->sink_backing + len, 0, FAKE_STDIO_BUF_SIZE - len);
	}

	fake_stdio->sink_len = len;
}

OSAL_STDIO *fake_stdio_stdin(void)
{
	get_stdin_call_count++;
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdin);
}

OSAL_STDIO *fake_stdio_stdout(void)
{
	get_stdout_call_count++;
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdout);
}

OSAL_STDIO *fake_stdio_stderr(void)
{
	get_stderr_call_count++;
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stderr);
}

size_t fake_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_t *fake;
	size_t requested_bytes;
	size_t available_bytes;
	size_t readable_bytes;
	size_t readable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);

	fake->read_call_count++;
	fake->last_read_ptr = ptr;
	fake->last_read_size = size;
	fake->last_read_nmemb = nmemb;
	fake->last_read_stream = stdio;

	if (fake->read_pos >= fake->buffered_len) {
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = fake->buffered_len - fake->read_pos;
	readable_bytes = (requested_bytes < available_bytes)
		? requested_bytes
		: available_bytes;

	readable_nmemb = readable_bytes / size;
	readable_bytes = readable_nmemb * size;

	if (readable_bytes > 0) {
		osal_memcpy(ptr, fake->buffered_backing + fake->read_pos, readable_bytes);
		fake->read_pos += readable_bytes;
	}

	return readable_nmemb;
}

size_t fake_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_t *fake;
	size_t requested_bytes;
	size_t available_bytes;
	size_t writable_bytes;
	size_t writable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);

	fake->write_call_count++;
	fake->last_write_ptr = ptr;
	fake->last_write_size = size;
	fake->last_write_nmemb = nmemb;
	fake->last_write_stream = stdio;

	if (fake->buffered_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = FAKE_STDIO_BUF_SIZE - fake->buffered_len;
	writable_bytes = (requested_bytes < available_bytes)
		? requested_bytes
		: available_bytes;

	writable_nmemb = writable_bytes / size;
	writable_bytes = writable_nmemb * size;

	if (writable_bytes > 0) {
		osal_memcpy(fake->buffered_backing + fake->buffered_len, ptr, writable_bytes);
		fake->buffered_len += writable_bytes;
	}

	return writable_nmemb;
}

int fake_stdio_flush(OSAL_STDIO *stdio)
{
	fake_stdio_t *fake;
	size_t flushable_bytes;
	size_t available_sink_bytes;

	if (!stdio) {
		return -1;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);

	fake->flush_call_count++;
	fake->last_flush_stream = stdio;

	if (fake->buffered_len == 0) {
		return 0;
	}

	if (fake->sink_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	available_sink_bytes = FAKE_STDIO_BUF_SIZE - fake->sink_len;
	flushable_bytes = (fake->buffered_len < available_sink_bytes)
		? fake->buffered_len
		: available_sink_bytes;

	if (flushable_bytes > 0) {
		osal_memcpy(
			fake->sink_backing + fake->sink_len,
			fake->buffered_backing,
			flushable_bytes);
		fake->sink_len += flushable_bytes;
	}

	if (flushable_bytes < fake->buffered_len) {
		osal_memmove(
			fake->buffered_backing,
			fake->buffered_backing + flushable_bytes,
			fake->buffered_len - flushable_bytes);
	}

	if (fake->buffered_len > flushable_bytes) {
		osal_memset(
			fake->buffered_backing + (fake->buffered_len - flushable_bytes),
			0,
			flushable_bytes);
	} else {
		osal_memset(fake->buffered_backing, 0, FAKE_STDIO_BUF_SIZE);
	}

	fake->buffered_len -= flushable_bytes;

	if (fake->read_pos > fake->buffered_len) {
		fake->read_pos = fake->buffered_len;
	}

	return 0;
}

int fake_stdio_error(OSAL_STDIO *stdio)
{
	(void)stdio;
	return 0;
}

int fake_stdio_eof(OSAL_STDIO *stdio)
{
	fake_stdio_t *fake;

	if (!stdio) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);
	return fake->read_pos >= fake->buffered_len;
}
