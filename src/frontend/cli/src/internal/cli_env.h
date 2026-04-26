

#ifndef LEXLEO_CLI_ENV_H
#define LEXLEO_CLI_ENV_H

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

typedef struct cli_env_t {
	const osal_mem_ops_t *mem_ops;
	const osal_stdio_ops_t *stdio_ops;
	const osal_file_ops_t *file_ops;
	const osal_str_ops_t *str_ops;
	const osal_time_ops_t *time_ops;

	// ...
} cli_env_t;

#endif //LEXLEO_CLI_ENV_H
