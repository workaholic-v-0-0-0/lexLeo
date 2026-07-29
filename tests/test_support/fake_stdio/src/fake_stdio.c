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

fake_stdio_ctrl_t g_fake_stdio_ctrl = {0};

void fake_stdio_init_instance(fake_stdio_t *fake_stdio)
{
	assert_non_null(fake_stdio);
	osal_memset(fake_stdio, 0, sizeof(*fake_stdio));
}

void fake_stdio_reset(void)
{
	osal_memset(&g_fake_stdio_ctrl, 0, sizeof(g_fake_stdio_ctrl));
	fake_stdio_init_instance(&g_fake_stdio_ctrl.stdin);
	fake_stdio_init_instance(&g_fake_stdio_ctrl.stdout);
	fake_stdio_init_instance(&g_fake_stdio_ctrl.stderr);
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

void fake_stdio_set_read_pos(
	fake_stdio_t *fake_stdio,
	size_t n
) {
	assert_non_null(fake_stdio);
	assert_true(n <= fake_stdio->buffered_len);
	fake_stdio->read_pos = n;
}

OSAL_STDIO *fake_get_stdio_stdin(void)
{
	g_fake_stdio_ctrl.get_stdin_call_count++;
	return fake_stdio_to_osal_stdio(&g_fake_stdio_ctrl.stdin);
}

OSAL_STDIO *fake_get_stdio_stdout(void)
{
	g_fake_stdio_ctrl.get_stdout_call_count++;
	return fake_stdio_to_osal_stdio(&g_fake_stdio_ctrl.stdout);
}

OSAL_STDIO *fake_get_stdio_stderr(void)
{
	g_fake_stdio_ctrl.get_stderr_call_count++;
	return fake_stdio_to_osal_stdio(&g_fake_stdio_ctrl.stderr);
}

size_t fake_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_t *fake_stdio;
	size_t requested_bytes;
	size_t available_bytes;
	size_t readable_bytes;
	size_t readable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake_stdio = osal_stdio_to_fake_stdio(stdio);

	fake_stdio->read_call_count++;
	fake_stdio->last_read_ptr = ptr;
	fake_stdio->last_read_size = size;
	fake_stdio->last_read_nmemb = nmemb;
	fake_stdio->last_read_stdio = stdio;

	if (fake_stdio->read_pos >= fake_stdio->buffered_len) {
		if (fake_stdio->end_of_input_has_been_signaled) {
			fake_stdio->end_of_input_has_been_encountered = true;
		}
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = fake_stdio->buffered_len - fake_stdio->read_pos;
	readable_bytes = (requested_bytes < available_bytes)
	   ? requested_bytes
	   : available_bytes;

	readable_nmemb = readable_bytes / size;
	readable_bytes = readable_nmemb * size;

	if (readable_bytes > 0) {
		osal_memcpy(
		   ptr,
		   fake_stdio->buffered_backing + fake_stdio->read_pos,
		   readable_bytes
		);
		fake_stdio->read_pos += readable_bytes;
	}

	if (
		  readable_nmemb < nmemb
	   && fake_stdio->end_of_input_has_been_signaled
	) {
		fake_stdio->end_of_input_has_been_encountered = true;
	}

	return readable_nmemb;
}

size_t fake_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_t *fake_stdio;
	size_t requested_bytes;
	size_t available_bytes;
	size_t writable_bytes;
	size_t writable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake_stdio = osal_stdio_to_fake_stdio(stdio);

	fake_stdio->write_call_count++;
	fake_stdio->last_write_ptr = ptr;
	fake_stdio->last_write_size = size;
	fake_stdio->last_write_nmemb = nmemb;
	fake_stdio->last_write_stdio = stdio;

	if (fake_stdio->buffered_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = FAKE_STDIO_BUF_SIZE - fake_stdio->buffered_len;
	writable_bytes = (requested_bytes < available_bytes)
		? requested_bytes
		: available_bytes;

	writable_nmemb = writable_bytes / size;
	writable_bytes = writable_nmemb * size;

	if (writable_bytes > 0) {
		osal_memcpy(fake_stdio->buffered_backing + fake_stdio->buffered_len, ptr, writable_bytes);
		fake_stdio->buffered_len += writable_bytes;
	}

	return writable_nmemb;
}

int fake_stdio_flush(OSAL_STDIO *stdio)
{
	fake_stdio_t *fake_stdio;
	size_t flushable_bytes;
	size_t available_sink_bytes;

	if (!stdio) {
		return -1;
	}

	fake_stdio = osal_stdio_to_fake_stdio(stdio);

	fake_stdio->flush_call_count++;
	fake_stdio->last_flush_stdio = stdio;

	if (fake_stdio->buffered_len == 0) {
		return 0;
	}

	if (fake_stdio->sink_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	available_sink_bytes = FAKE_STDIO_BUF_SIZE - fake_stdio->sink_len;
	flushable_bytes = (fake_stdio->buffered_len < available_sink_bytes)
		? fake_stdio->buffered_len
		: available_sink_bytes;

	if (flushable_bytes > 0) {
		osal_memcpy(
			fake_stdio->sink_backing + fake_stdio->sink_len,
			fake_stdio->buffered_backing,
			flushable_bytes);
		fake_stdio->sink_len += flushable_bytes;
	}

	if (flushable_bytes < fake_stdio->buffered_len) {
		osal_memmove(
			fake_stdio->buffered_backing,
			fake_stdio->buffered_backing + flushable_bytes,
			fake_stdio->buffered_len - flushable_bytes);
	}

	if (fake_stdio->buffered_len > flushable_bytes) {
		osal_memset(
			fake_stdio->buffered_backing + (fake_stdio->buffered_len - flushable_bytes),
			0,
			flushable_bytes);
	} else {
		osal_memset(fake_stdio->buffered_backing, 0, FAKE_STDIO_BUF_SIZE);
	}

	fake_stdio->buffered_len -= flushable_bytes;

	if (fake_stdio->read_pos > fake_stdio->buffered_len) {
		fake_stdio->read_pos = fake_stdio->buffered_len;
	}

	if (fake_stdio->next_flush_ret != 0)
		return fake_stdio->next_flush_ret;

	return 0;
}

int fake_stdio_error(OSAL_STDIO *stdio)
{
	assert_non_null(stdio);
	fake_stdio_t *fake_stdio = osal_stdio_to_fake_stdio(stdio);
	fake_stdio->error_call_count++;
	fake_stdio->last_error_stdio = stdio;
	return fake_stdio->next_error_ret;
}

int fake_stdio_eof(OSAL_STDIO *stdio)
{
	assert_non_null(stdio);
	fake_stdio_t *fake_stdio = osal_stdio_to_fake_stdio(stdio);
	fake_stdio->eof_call_count++;
	fake_stdio->last_eof_stdio = stdio;
	return fake_stdio->end_of_input_has_been_encountered;
}
