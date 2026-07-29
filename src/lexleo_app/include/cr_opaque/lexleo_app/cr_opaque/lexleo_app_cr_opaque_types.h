

#ifndef LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H
#define LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H

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

#endif /* LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H */
