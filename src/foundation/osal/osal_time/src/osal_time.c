

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
