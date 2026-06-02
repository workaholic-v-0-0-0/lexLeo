/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_io_creator_handle.h
 * @ingroup stream_internal_group
 * @brief Private handle for I/O stream creators.
 */

#ifndef LEXLEO_STREAM_IO_CREATOR_HANDLE_H
#define LEXLEO_STREAM_IO_CREATOR_HANDLE_H

#include "internal/stream_creator_handle.h"

struct stream_io_creator_t {
	stream_creator_generic_t base;
};

#endif /* LEXLEO_STREAM_IO_CREATOR_HANDLE_H */