/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "internal/cli_log_path.h"

#include "osal/stdio/osal_stdio.h"

#include "policy/lexleo_cstd_lib.h"

bool cli_resolve_default_log_path(char *buf, size_t buf_size)
{
	if (!buf || buf_size == 0)
		return false;

	const char *home = getenv("HOME");
	if (!home || !*home)
		return false;

	int ret =
		osal_snprintf(
			buf,
			buf_size,
			"%s/.local/state/lexleo/lexleo.log",
			home
		);

	if (ret < 0 || (size_t)ret >= buf_size)
		return false;

	return true;
}
