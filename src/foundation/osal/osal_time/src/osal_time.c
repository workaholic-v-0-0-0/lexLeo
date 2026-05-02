/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_time.c
 * @ingroup osal_time_internal_group
 * @brief Private implementation of the `osal_time` module.
 *
 * @details
 * This file implements the default OSAL time operations and their
 * underlying platform-backed behavior.
 */

#include "osal/time/osal_time_ops.h"

#include "policy/lexleo_cstd_time.h"

osal_time_status_t osal_time_now_impl(osal_time_t *out)
{
	if (!out) {
		return OSAL_TIME_STATUS_INVALID;
	}

	time_t now = time(NULL);
	if (now == (time_t)-1) {
		return OSAL_TIME_STATUS_ERROR;
	}

	out->epoch_seconds = (int64_t)now;
	return OSAL_TIME_STATUS_OK;
}

const osal_time_ops_t *osal_time_default_ops(void)
{
	static const osal_time_ops_t OPS = {
		.now = osal_time_now_impl
	};
	return &OPS;
}
