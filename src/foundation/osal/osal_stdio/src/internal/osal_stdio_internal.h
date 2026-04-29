/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_stdio_internal.h
 * @ingroup osal_stdio_internal_group
 * @brief Private internal declarations for the `osal_stdio` module.
 *
 * @details
 * This header exposes backend-private runtime structures used by the default
 * `osal_stdio` implementation.
 */

#ifndef LEXLEO_OSAL_STDIO_INTERNAL_H
#define LEXLEO_OSAL_STDIO_INTERNAL_H

#include "policy/lexleo_cstd_io.h"

struct OSAL_STDIO {
	FILE *fp;
};

#endif /* LEXLEO_OSAL_STDIO_INTERNAL_H */
