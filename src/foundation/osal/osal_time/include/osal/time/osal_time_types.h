/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_time_types.h
 * @ingroup osal_time_api
 * @brief Public time types of the `osal_time` module.
 *
 * @details
 * This header declares public types used by the OSAL time module.
 *
 * Current public types:
 * - `osal_time_ops_t`
 * - `osal_time_t`
 * - `osal_time_status_t`
 */

#ifndef LEXLEO_OSAL_TIME_TYPES_H
#define LEXLEO_OSAL_TIME_TYPES_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_time_ops_t osal_time_ops_t;

/**
 * @brief Public OSAL wall-clock time value.
 *
 * @details
 * Represents wall-clock time as seconds since the Unix epoch.
 */
typedef struct {
	int64_t epoch_seconds;
} osal_time_t;

/**
 * @brief Status codes returned by OSAL time operations.
 */
typedef enum osal_time_status_t {
	OSAL_TIME_STATUS_OK = 0,
	OSAL_TIME_STATUS_INVALID,
	OSAL_TIME_STATUS_ERROR
} osal_time_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_TIME_TYPES_H */
