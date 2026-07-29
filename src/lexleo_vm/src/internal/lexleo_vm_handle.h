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

#include "stream/common/stream_opaque_type.h"

#include "logger/common/logger_opaque_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lexleo_vm_owned_resources_t lexleo_vm_owned_resources_t;

/**
 * @brief Private LexLeo VM handle structure.
 *
 * @details
 * This structure stores the borrowed dependencies injected at creation time
 * and the owned runtime resources initialized later by
 * `lexleo_vm_complete_default_init()`.
 */
struct lexleo_vm_t {

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
	lexleo_vm_owned_resources_t *lexleo_vm_owned_resources;
};

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_VM_HANDLE_H */
