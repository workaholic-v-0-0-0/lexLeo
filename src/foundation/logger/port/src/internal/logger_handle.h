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

#include "logger/adapters/logger_adapters_vtbl.h"

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private handle structure for a `logger_t`.
 *
 * @details
 * This structure is the private in-memory handle bound to a public `logger_t`
 * object.
 */
struct logger_t {

	/** Borrowed adapter dispatch table. */
	const logger_vtbl_t *vtbl;

	/** Borrowed memory operations used to destroy the handle. */
	const osal_mem_ops_t *mem;

	/** Owned opaque backend instance destroyed through `vtbl->destroy`. */
	void *backend;

};

#endif /* LEXLEO_LOGGER_HANDLE_H */
