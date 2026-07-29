/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_str.c
 * @ingroup test_support_fake_str
 * @brief Test double implementation for injectable OSAL string operations.
 */

#include "lexleo/test/fake_str.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_cstd_types.h"

#include "lexleo_cmocka.h"

static fake_str_ctrl_t g_fake_str_ctrl_impl = {0};
fake_str_ctrl_t *g_fake_str_ctrl = &g_fake_str_ctrl_impl;

void fake_str_reset(void)
{
	osal_memset(g_fake_str_ctrl, 0, sizeof(g_fake_str_ctrl_impl));
}

char *fake_str_strdup (const char *s, const osal_mem_ops_t *mem_ops)
{
	assert_non_null(s);
	assert_non_null(mem_ops);
	assert_non_null(mem_ops->calloc);

	g_fake_str_ctrl->strdup_call_count++;
	g_fake_str_ctrl->last_s = s;
	g_fake_str_ctrl->last_mem_ops = mem_ops;

	if (g_fake_str_ctrl->next_strdup_will_fail) {
		return NULL;
	}

	size_t size = sizeof(char) * (osal_strlen(s) + 1);
	char *ret = mem_ops->calloc(1, size);
	if (!ret) {
		return NULL;
	}

	return osal_memcpy(ret, s, size);
}
