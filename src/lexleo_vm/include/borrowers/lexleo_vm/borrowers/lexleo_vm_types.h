/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_types.h
 * @ingroup lexleo_vm_borrowers_api
 * @brief Common public types for the LexLeo VM module.
 *
 * @details
 * This header declares the opaque VM handle and the status codes returned by
 * the public LexLeo VM API.
 */

#ifndef LEXLEO_LEXLEO_VM_TYPES_H
#define LEXLEO_LEXLEO_VM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque LexLeo VM handle.
 *
 * @details
 * The structure definition is private to the module. Clients manipulate VM
 * instances only through pointers returned by the public API.
 */
typedef struct lexleo_vm_t lexleo_vm_t;

/**
 * @brief Status codes returned by the LexLeo VM API.
 */
typedef enum {
	/** Operation completed successfully. */
	LEXLEO_VM_STATUS_OK,

	/** Memory allocation failed. */
	LEXLEO_VM_STATUS_OOM,

	/** Memory allocation failed while creating the default stream factory. */
	LEXLEO_VM_STATUS_STREAM_FACTORY_INIT_OOM,

	/** Memory allocation failed while creating the default stdio stream
	 * creator. */
	LEXLEO_VM_STATUS_STREAM_IO_CREATOR_INIT_OOM,

	/** Memory allocation failed while creating the default file stream
	 * creator. */
	LEXLEO_VM_STATUS_STREAM_FILE_CREATOR_INIT_OOM,

	/** Memory allocation failed while creating the default buffer stream
	 * creator. */
	LEXLEO_VM_STATUS_STREAM_BUFFER_CREATOR_INIT_OOM
} lexleo_vm_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_VM_TYPES_H */
