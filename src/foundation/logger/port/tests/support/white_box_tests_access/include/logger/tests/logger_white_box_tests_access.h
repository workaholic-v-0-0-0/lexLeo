/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_white_box_tests_access.h
 * @ingroup logger_white_box_tests_access_group
 * @brief Privileged white-box test access to internal logger state.
 *
 * @details
 * This header provides test-only access to selected internal  logger state.
 *
 * These declarations are not part of the production API.
 */

#ifndef LOGGER_WHITE_BOX_TESTS_ACCESS_H
#define LOGGER_WHITE_BOX_TESTS_ACCESS_H

#include "logger/owners/logger_owners_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void logger_inject_backend(logger_t *logger, void *backend);

void *logger_get_backend(const logger_t *logger);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_WHITE_BOX_TESTS_ACCESS_H */
