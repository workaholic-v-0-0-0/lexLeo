#include "osal/time/test/osal_time_fake_provider.h"

const osal_time_ops_t *osal_time_test_fake_ops(void)
{
	static const osal_time_ops_t OPS = {
		.now = fake_time_now
	};
	return &OPS;
}
