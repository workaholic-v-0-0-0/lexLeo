#include "lexleo/test/fake_time.h"

typedef struct {
	/* cfg */
	osal_time_status_t now_status;
	osal_time_t now_out;

	/* spy */
	size_t now_call_count;
} fake_time_t;

static fake_time_t g_fake_time;

void fake_time_reset(void) {
	g_fake_time.now_status = OSAL_TIME_STATUS_OK;
	g_fake_time.now_out.epoch_seconds = 0;
	g_fake_time.now_call_count = 0;
}

void fake_time_set_now_status(osal_time_status_t status) {
	g_fake_time.now_status = status;
}

void fake_time_set_now_out(osal_time_t out) {
	g_fake_time.now_out = out;
}

size_t fake_time_get_call_count(void) {
	return g_fake_time.now_call_count;
}

osal_time_status_t fake_time_now(osal_time_t *out) {
	g_fake_time.now_call_count++;
	if (out == NULL) return OSAL_TIME_STATUS_INVALID;
	if (g_fake_time.now_status != OSAL_TIME_STATUS_OK) return g_fake_time.now_status;
	*out = g_fake_time.now_out;
	return g_fake_time.now_status;
}
