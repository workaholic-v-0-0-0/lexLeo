/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_env.c
 * @ingroup osal_env_internal_group
 * @brief Default implementation of the `osal_env` module.
 *
 * @details
 * This translation unit provides the backend implementation of the public
 * environment access services declared by `osal_env.h`.
 *
 * The current implementation delegates environment queries to the active
 * standard-library backend selected through policy headers.
 */

#include "policy/lexleo_cstd_lib.h"

const char *osal_getenv(const char* name)
{
	return getenv(name);
}
