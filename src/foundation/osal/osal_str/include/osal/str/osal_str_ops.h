/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_str_ops.h
 * @ingroup osal_str_api
 * @brief Injectable string operations of the `osal_str` module.
 *
 * @details
 * This header declares the injectable operations table used by modules that
 * need string services with explicit dependency injection.
 *
 * Current injectable services:
 * - string duplication through a caller-provided memory provider
 */

#ifndef LEXLEO_OSAL_STR_OPS_H
#define LEXLEO_OSAL_STR_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal/str/osal_str_types.h"

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

struct osal_str_ops_t {
	char *(*strdup) (const char *s, const osal_mem_ops_t *mem_ops);
};

const osal_str_ops_t *osal_str_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STR_OPS_H
