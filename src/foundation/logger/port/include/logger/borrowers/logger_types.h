/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_LOGGER_TYPES_H
#define LEXLEO_LOGGER_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct logger_t logger_t;

typedef enum {
	LOGGER_STATUS_OK = 0,
	LOGGER_STATUS_INVALID,
	LOGGER_STATUS_IO_ERROR,
	LOGGER_STATUS_NO_BACKEND,
	LOGGER_STATUS_OOM
} logger_status_t;

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LOGGER_TYPES_H
