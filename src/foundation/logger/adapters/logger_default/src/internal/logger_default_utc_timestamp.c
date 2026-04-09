/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_utc_timestamp.c
 * @ingroup logger_default_internal_group
 * @brief Private UTC timestamp conversion helpers for the `logger_default` adapter.
 *
 * @details
 * This file implements private helpers used to convert epoch-time values into
 * decomposed UTC+0 civil timestamps for `logger_default` log formatting.
 */

#include "internal/logger_default_utc_timestamp.h"

#include "osal/time/osal_time_types.h"

/**
 * @brief Return whether a Gregorian calendar year is leap.
 *
 * @details
 * A year is leap if it is divisible by 4, except century years that are not
 * divisible by 400.
 *
 * @param[in] year
 * Gregorian calendar year.
 *
 * @retval true
 * `year` is a leap year.
 *
 * @retval false
 * `year` is not a leap year.
 */
static bool logger_default_is_leap_year(int32_t year)
{
	return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

/**
 * @brief Return the number of days in a Gregorian calendar year.
 *
 * @param[in] year
 * Gregorian calendar year.
 *
 * @return
 * `366` if `year` is leap, otherwise `365`.
 */
static int32_t logger_default_days_in_year(int32_t year)
{
	return logger_default_is_leap_year(year) ? 366 : 365;
}

/**
 * @brief Return the number of days in a Gregorian calendar month.
 *
 * @param[in] year
 * Gregorian calendar year.
 *
 * @param[in] month
 * Gregorian calendar month in the range `[1, 12]`.
 *
 * @return
 * Number of days in the requested month for the requested year.
 */
static int32_t logger_default_days_in_month(int32_t year, int32_t month)
{
	static const int32_t DAYS_PER_MONTH[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	int32_t days = DAYS_PER_MONTH[month - 1];

	if (month == 2 && logger_default_is_leap_year(year)) {
		days = 29;
	}

	return days;
}

/**
 * @brief Convert an epoch-time value to a decomposed UTC timestamp.
 *
 * @details
 * This helper interprets `time->epoch_seconds` as a UTC+0 Unix epoch-time value
 * and converts it into calendar and clock fields stored in `*out`.
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
	const osal_time_t *time)
{
	if (!out || !time) {
		return false;
	}

	int64_t epoch_seconds = time->epoch_seconds;

	int64_t days = epoch_seconds / 86400;
	int64_t rem = epoch_seconds % 86400;

	if (rem < 0) {
		rem += 86400;
		days -= 1;
	}

	out->hour = (int32_t)(rem / 3600);
	rem %= 3600;
	out->minute = (int32_t)(rem / 60);
	out->second = (int32_t)(rem % 60);

	int32_t year = 1970;

	if (days >= 0) {
		while (days >= logger_default_days_in_year(year)) {
			days -= logger_default_days_in_year(year);
			year++;
		}
	} else {
		do {
			year--;
			days += logger_default_days_in_year(year);
		} while (days < 0);
	}

	out->year = year;

	int32_t month = 1;
	while (days >= logger_default_days_in_month(year, month)) {
		days -= logger_default_days_in_month(year, month);
		month++;
	}

	out->month = month;
	out->day = (int32_t)days + 1;

	return true;
}
