/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_dynamic_buffer_creator_type.h
 * @ingroup stream_internal_group
 * @brief Private dynamic buffer stream creator type.
 *
 * @details
 * Defines the private representation of
 * `stream_dynamic_buffer_creator_t`.
 */

#ifndef LEXLEO_STREAM_DYNAMIC_BUFFER_CREATOR_TYPE_H
#define LEXLEO_STREAM_DYNAMIC_BUFFER_CREATOR_TYPE_H

typedef struct stream_dynamic_buffer_creator_t {
	stream_creator_generic_t base;
} stream_dynamic_buffer_creator_t;

#endif /* LEXLEO_STREAM_DYNAMIC_BUFFER_CREATOR_TYPE_H */
