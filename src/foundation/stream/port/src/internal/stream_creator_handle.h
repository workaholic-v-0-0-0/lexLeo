/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_creator_handle.h
 * @ingroup stream_internal_group
 * @brief Private base handle shared by `stream` creator implementations.
 *
 * @details
 * Defines the common private state shared by specialized `stream` creator
 * implementations.
 */

#ifndef LEXLEO_STREAM_CREATOR_HANDLE_H
#define LEXLEO_STREAM_CREATOR_HANDLE_H

#include "stream/common/stream_factory_opaque_type.h"
#include "stream/common/stream_adapter_id_type.h"

#include "osal/mem/osal_mem_types.h"

/**
 * @brief Private base handle shared by `stream` creator implementations.
 */
typedef struct stream_creator_generic_t {

	/** Borrowed stream factory. */
	stream_factory_t *factory;

	/** Backend provider key used for stream creation. */
	stream_adapter_id_t adapter_id;

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem;

} stream_creator_generic_t;

#endif /* LEXLEO_STREAM_CREATOR_HANDLE_H */
