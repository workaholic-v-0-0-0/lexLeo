
// WIP todo

#include "../internal/lexleo_app_handle.h"
#include "../internal/lexleo_app_cr_internal.h"
#include "../internal/lexleo_app_log_path.h"

#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/str/osal_str.h"
#include "osal/time/osal_time_ops.h"

#include "stream/cr/stream_cr_api.h"
#include "stream/adapters/stream_adapters_types.h"
#include "fs_stream/cr/fs_stream_cr_api.h"
#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "logger/cr/logger_cr_api.h"
#include "logger/adapters/logger_adapters_types.h"
#include "logger_default/cr/logger_default_cr_api.h"

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "policy/lexleo_assert.h"

// <here> unit tests are ready now ; just have to implement ; take care about lexleo_app_default_cfg() test is "false green"
lexleo_app_cfg_t lexleo_app_default_cfg(void)
{
	return (lexleo_app_cfg_t){0}; // placeholder
}

lexleo_app_cr_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env
) {
	return LEXLEO_APP_CR_STATUS_OK; // placeholder
}

void lexleo_app_destroy(lexleo_app_t **app)
{
	return; // placeholder
}

lexleo_app_cr_status_t lexleo_app_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg
) {
	return LEXLEO_APP_CR_STATUS_OK; // placeholder
}

bool lexleo_app_create_init(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg
) {
	return true; // placeholder
}
