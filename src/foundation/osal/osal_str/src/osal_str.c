/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_str.c
 * @ingroup osal_str_internal_group
 * @brief Private implementation of the `osal_str` module.
 *
 * @details
 * This file implements the public OSAL string helpers and the default
 * injectable string operations table.
 */

#include "osal/str/osal_str.h"
#include "osal/str/osal_str_ops.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstring.h"
#include "policy/lexleo_cstd_ctype.h"
#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_assert.h"

static char *osal_strdup(
	const char *s,
	const osal_mem_ops_t *mem_ops
) {
	if (!s || !mem_ops) {
		return NULL;
	}

	LEXLEO_ASSERT(mem_ops->malloc);

	size_t n = strlen(s) + 1;

	char *p = mem_ops->malloc(n);
	if (!p) return NULL;

	osal_memcpy(p, s, n);
	return p;
}

const osal_str_ops_t *osal_str_default_ops(void)
{
	static const osal_str_ops_t OPS = {
		.strdup =  osal_strdup,
	};
	return &OPS;
}

size_t osal_strlen(const char *s)
{
	return strlen(s);
}

int osal_strcmp(const char *s1, const char *s2) {
	return strcmp(s1, s2);
}

char *osal_strchr(const char *s, int c)
{
	return strchr(s, c);
}

char *osal_strrchr(const char *s, int c)
{
	return strrchr(s, c);
}

int osal_isspace(int c)
{
	return isspace(c);
}

int osal_snprintf(
	char *str,
	size_t size,
	const char *format,
	...
) {
	va_list args;
	int ret;

	if (!format || (size > 0 && !str)) {
		return -1;
	}

	va_start(args, format);
	ret = vsnprintf(str, size, format, args);
	va_end(args);

	return ret;
}
