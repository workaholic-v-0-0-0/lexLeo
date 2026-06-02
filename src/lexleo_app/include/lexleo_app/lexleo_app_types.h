

#ifndef LEXLEO_LEXLEO_APP_TYPES_H
#define LEXLEO_LEXLEO_APP_TYPES_H

typedef struct lexleo_app_t lexleo_app_t;

typedef enum lexleo_app_status_t {
	LEXLEO_APP_STATUS_OK,
	LEXLEO_APP_STATUS_OOM,
	LEXLEO_APP_STATUS_LOG_PATH_RESOLUTION_ERROR,
	LEXLEO_APP_STATUS_STREAM_ERROR,
	LEXLEO_APP_STATUS_INPUT_INIT_ERROR,
	LEXLEO_APP_STATUS_OUTPUT_INIT_ERROR,
	LEXLEO_APP_STATUS_ERR_INIT_ERROR,
	LEXLEO_APP_STATUS_LOGGER_STREAM_INIT_ERROR,
	LEXLEO_APP_STATUS_LOGGER_INIT_ERROR,
	LEXLEO_APP_STATUS_VM_INIT_ERROR
	// ...
} lexleo_app_status_t;

typedef enum lexleo_app_io_kind_t {
	LEXLEO_APP_IO_STDIO,
	LEXLEO_APP_IO_FILE,
	LEXLEO_APP_IO_BUFFER
} lexleo_app_io_kind_t;

typedef struct lexleo_app_io_cfg_t {
	lexleo_app_io_kind_t kind;
	const char *path; /* only for FILE */
	const char *mode; /* "rb", "wb" or "ab" */
} lexleo_app_io_cfg_t;

typedef struct lexleo_app_cfg_t {
	lexleo_app_io_cfg_t in;
	lexleo_app_io_cfg_t out;
	lexleo_app_io_cfg_t err;
} lexleo_app_cfg_t;

#endif /* LEXLEO_LEXLEO_APP_TYPES_H */
