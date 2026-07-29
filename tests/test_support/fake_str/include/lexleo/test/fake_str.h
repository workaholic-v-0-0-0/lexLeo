/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_str.h
 * @ingroup test_support_fake_str
 * @brief Test double API for injectable OSAL string operations.
 */

#ifndef LEXLEO_FAKE_STR_H
#define LEXLEO_FAKE_STR_H

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fake_str_ctrl_t {

	/* cfg */
	bool next_strdup_will_fail;

	/* spy */
	size_t strdup_call_count;
	const char *last_s;
	const osal_mem_ops_t *last_mem_ops;

} fake_str_ctrl_t;

extern fake_str_ctrl_t *g_fake_str_ctrl;

void fake_str_reset(void);

char *fake_str_strdup(const char *s, const osal_mem_ops_t *mem_ops);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_STR_H */
