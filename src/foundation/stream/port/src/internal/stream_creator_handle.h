/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_creator_handle.h
 * @ingroup stream_internal_group
 * @brief Private base handle shared by `stream` creator implementations.
 *
 * @details
 * This header defines the common private state shared by specialized
 * `stream` creator objects, including their CR-managed lifetime code and
 * owner-facing creation operations.
 */

#ifndef LEXLEO_STREAM_CREATOR_HANDLE_H
#define LEXLEO_STREAM_CREATOR_HANDLE_H

#include "stream/owners/stream_owners_types.h"

#include "osal/mem/osal_mem_types.h"

/**
 * @brief Private base handle shared by `stream` creator implementations.
 */
typedef struct stream_creator_generic_t {

	/** Borrowed stream factory. */
	stream_factory_t *factory;

	/** Adapter key used for stream creation. */
	stream_key_t key;

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem;

} stream_creator_generic_t;

#endif /* LEXLEO_STREAM_CREATOR_HANDLE_H */
