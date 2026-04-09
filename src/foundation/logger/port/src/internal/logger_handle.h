/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_handle.h
 * @ingroup logger_internal_group
 * @brief Private logger handle definition for the `logger` port.
 *
 * @details
 * This header exposes the private handle structure used by the `logger` port
 * implementation.
 */

#ifndef LEXLEO_LOGGER_HANDLE_H
#define LEXLEO_LOGGER_HANDLE_H

#include "logger/adapters/logger_adapters_api.h"

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private handle structure for a `logger_t`.
 *
 * @details
 * This structure is the private in-memory handle bound to a public `logger_t`
 * object.
 *
 * It stores:
 * - the bound adapter dispatch table,
 * - the opaque backend instance,
 * - the memory operations used for destruction.
 */
struct logger_t {
	/** Bound adapter dispatch table. */
	logger_vtbl_t vtbl;

	/** Opaque adapter-owned backend instance. */
	void *backend;

	/** Memory operations used to destroy the handle. */
	const osal_mem_ops_t *mem;
};

#endif // LEXLEO_LOGGER_HANDLE_H
