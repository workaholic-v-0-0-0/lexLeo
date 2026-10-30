/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_white_box_tests_access.h
 * @ingroup stream_white_box_tests_access_group
 * @brief Privileged white-box access to internal `stream` state and
 * operations for unit tests.
 *
 * @details
 * Declares test-only helpers used to inspect and modify private `stream_t`
 * state and to access selected internal stream operations without exposing
 * private implementation headers to test translation units.
 */

#ifndef STREAM_WHITE_BOX_TESTS_ACCESS_H
#define STREAM_WHITE_BOX_TESTS_ACCESS_H

#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_vtbl_type.h"
#include "stream/cr/stream_cr_types.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inject an adapter backend into a stream handle.
 *
 * @param[in,out] stream Stream handle to modify.
 * @param[in,out] backend Adapter backend to inject.
 */
void stream_inject_backend(
	stream_t *stream,
	void *backend
);

/**
 * @brief Return the dispatch table bound to a stream handle.
 *
 * @param[in] stream Stream handle to inspect.
 *
 * @return Bound stream adapter dispatch table.
 */
const stream_vtbl_t *stream_get_vtbl(
	const stream_t *stream
);

/**
 * @brief Return the memory operations bound to a stream handle.
 *
 * @param[in] stream Stream handle to inspect.
 *
 * @return Bound memory operations.
 */
const osal_mem_ops_t *stream_get_mem(
	const stream_t *stream
);

/**
 * @brief Return the backend bound to a stream handle.
 *
 * @param[in] stream Stream handle to inspect.
 *
 * @return Bound adapter backend.
 */
void *stream_get_backend(
	const stream_t *stream
);

/**
 * @brief Test-facing view of selected internal stream operations.
 *
 * @details
 * Provides indirect access to internal operations required by white-box unit
 * tests without requiring test translation units to include private stream
 * implementation headers.
 */
typedef struct stream_internal_api_t {

	/** Construct a stream using a registered adapter provider. */
	stream_factory_status_t (*stream_factory_create_stream)(
		const stream_factory_t *factory,
		stream_adapter_id_t adapter_id,
		const void *args,
		stream_t **out
	);

} stream_internal_api_t;

/**
 * @brief Return the test-facing internal stream API.
 *
 * @return Pointer to the internal stream API dispatch table.
 */
const stream_internal_api_t *stream_get_internal_api(void);

#ifdef __cplusplus
}
#endif

#endif /* STREAM_WHITE_BOX_TESTS_ACCESS_H */
