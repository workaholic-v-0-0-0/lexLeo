

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
