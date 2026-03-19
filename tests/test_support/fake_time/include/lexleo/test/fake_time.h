#ifndef LEXLEO_FAKE_TIME_H
#define LEXLEO_FAKE_TIME_H

#include "osal/time/osal_time_types.h"
#include "policy/lexleo_cstd_types.h"

osal_time_status_t fake_time_now(osal_time_t *out);

void fake_time_reset(void);

void fake_time_set_now_status(osal_time_status_t status);
void fake_time_set_now_out(osal_time_t out);

size_t fake_time_get_call_count(void);

#endif //LEXLEO_FAKE_TIME_H
