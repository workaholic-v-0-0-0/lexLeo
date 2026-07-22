/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_handle.h
 * @ingroup lexleo_vm_internal_group
 * @brief Private LexLeo VM handle definition.
 *
 * @details
 * This header defines the internal structure of the opaque `lexleo_vm_t`
 * handle.
 *
 * The handle stores borrowed runtime dependencies provided at creation time
 * and owned runtime resources created during default initialization.
 *
 * This header is private to the `lexleo_vm` implementation and must not be
 * included by public API clients.
 */

#ifndef LEXLEO_LEXLEO_VM_HANDLE_H
#define LEXLEO_LEXLEO_VM_HANDLE_H

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/borrowers/stream_borrowers_types.h"
#include "stream/owners/stream_owners_types.h"
#include "stream/owners/stream_creators_api.h"

#include "logger/borrowers/logger_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private LexLeo VM handle structure.
 *
 * @details
 * This structure stores the borrowed dependencies injected at creation time
 * and the owned runtime resources initialized later by
 * `lexleo_vm_complete_default_init()`.
 */
struct lexleo_vm_t {

	/* Borrowed dependencies. */

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

	/** Borrowed standard I/O operations. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed file operations. */
	const osal_file_ops_t *file_ops;

	/** Borrowed string operations. */
	const osal_str_ops_t *str_ops;

	/** Borrowed time operations. */
	const osal_time_ops_t *time_ops;

	/** Borrowed input stream. */
	stream_t *in;

	/** Borrowed output stream. */
	stream_t *out;

	/** Borrowed error stream. */
	stream_t *err;

	/** Borrowed logger. */
	logger_t *logger;


	/* Owned runtime resources. */

	/** Owned stream factory. */
	stream_factory_t *stream_factory;

	/** Owned stdio stream creator. */
	stream_io_creator_t *stream_io_creator;

	/** Owned file stream creator. */
	stream_file_creator_t *stream_file_creator;

	/** Owned buffer stream creator. */
	stream_buffer_creator_t *stream_buffer_creator;
};

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_VM_HANDLE_H */
