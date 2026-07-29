

#ifndef LEXLEO_LEXLEO_APP_HANDLE_H
#define LEXLEO_LEXLEO_APP_HANDLE_H

#include "internal/lexleo_app_log_path.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/common/stream_opaque_type.h"

#include "logger/common/logger_opaque_type.h"

#include "lexleo_vm/borrowers/lexleo_vm_borrowers_types.h"

struct lexleo_app_t {

	const osal_mem_ops_t *mem_ops;
	const osal_stdio_ops_t *stdio_ops;
	const osal_file_ops_t *file_ops;
	const osal_str_ops_t *str_ops;
	const osal_time_ops_t *time_ops;

	stream_t *in;
	stream_t *out;
	stream_t *err;

	char log_path[LEXLEO_APP_LOG_PATH_BUFFER_SIZE];
	stream_t *logger_stream;
	logger_t *logger;

	lexleo_vm_t *vm;

	// ...

};

#endif /* LEXLEO_LEXLEO_APP_HANDLE_H */
