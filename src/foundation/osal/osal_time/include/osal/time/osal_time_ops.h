/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_time_ops.h
 * @ingroup osal_time_api
 * @brief Injectable time operations of the `osal_time` module.
 *
 * @details
 * This header declares the injectable operations table used by modules that
 * need time services with explicit dependency injection.
 *
 * Current injectable services:
 * - current time retrieval through `now`
 */

#ifndef LEXLEO_OSAL_TIME_OPS_H
#define LEXLEO_OSAL_TIME_OPS_H

#include "osal/time/osal_time_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct osal_time_ops_t {
	osal_time_status_t (*now)(osal_time_t *out);
};

const osal_time_ops_t *osal_time_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_TIME_OPS_H
