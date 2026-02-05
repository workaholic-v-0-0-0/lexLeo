/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/internal/
 * logger_default_state.h
 */

#ifndef LEXLEO_LOGGER_DEFAULT_STATE_H
#define LEXLEO_LOGGER_DEFAULT_STATE_H

typedef struct stream_t stream_t;

typedef struct logger_default_state_t {
	stream_t *stream;
} logger_default_state_t;

#endif //LEXLEO_LOGGER_DEFAULT_STATE_H