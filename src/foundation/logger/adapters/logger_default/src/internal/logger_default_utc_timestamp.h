/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_utc_timestamp.h
 * @ingroup logger_default_internal_group
 * @brief Private UTC timestamp conversion helpers for the `logger_default` adapter.
 *
 * @details
 * This header exposes private timestamp-related types and helpers used by the
 * `logger_default` adapter implementation.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_UTC_TIMESTAMP_H
#define LEXLEO_LOGGER_DEFAULT_UTC_TIMESTAMP_H

#include "policy/lexleo_cstd_types.h"
#include "osal/time/osal_time_types.h"

/**
 * @brief Private UTC timestamp representation used by `logger_default`.
 *
 * @details
 * This structure stores a UTC+0 civil timestamp decomposed into calendar and
 * clock fields.
 * It is produced internally from an `osal_time_t` epoch-time value for log
 * formatting purposes.
 */
typedef struct logger_default_utc_timestamp_t {
	/** UTC year component. */
	int32_t year;

	/** UTC month component in the range [1, 12]. */
	int32_t month;

	/** UTC day-of-month component in the range [1, 31]. */
	int32_t day;

	/** UTC hour component in the range [0, 23]. */
	int32_t hour;

	/** UTC minute component in the range [0, 59]. */
	int32_t minute;

	/** UTC second component in the range [0, 59]. */
	int32_t second;
} logger_default_utc_timestamp_t;

/**
 * @brief Convert an epoch-time value to a decomposed UTC timestamp.
 *
 * @details
 * This private helper converts `time->epoch_seconds` into a UTC+0 civil
 * timestamp stored in `*out`.
 *
 * @param[out] out
 * Receives the converted UTC timestamp.
 * Must not be `NULL`.
 *
 * @param[in] time
 * Epoch-time value to convert.
 * Must not be `NULL`.
 *
 * @retval true
 * Conversion succeeded and `*out` was populated.
 *
 * @retval false
 * One or more arguments are invalid.
 */
bool logger_default_epoch_time_to_date(
	logger_default_utc_timestamp_t *out,
	const osal_time_t *time);

#endif //LEXLEO_LOGGER_DEFAULT_UTC_TIMESTAMP_H
