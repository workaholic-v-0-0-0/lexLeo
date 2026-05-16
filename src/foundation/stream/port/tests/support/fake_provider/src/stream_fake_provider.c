/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_provider.c
 * @ingroup stream_tests_group
 * @brief Fake stream provider implementation for stream tests.
 */

#include "stream/test/stream_fake_provider.h"

const stream_buffer_creator_t *stream_test_fake_buffer_creator(void)
{
	return fake_stream_buffer_creator();
}

const stream_file_creator_t *stream_test_fake_file_creator(void)
{
	return fake_stream_file_creator();
}

const stream_io_creator_t *stream_test_fake_io_creator(void)
{
	return fake_stream_io_creator();
}
