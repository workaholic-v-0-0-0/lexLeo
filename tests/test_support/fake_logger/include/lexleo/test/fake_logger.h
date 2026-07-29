/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_logger.h
 * @ingroup test_support_fake_logger
 * @brief Fake logger support for unit tests.
 *
 * @details
 * bbb
 */
//<here> adjust this file
#ifndef LEXLEO_FAKE_LOGGER_H
#define LEXLEO_FAKE_LOGGER_H

#include "logger/owners/logger_owners_api.h"

#include "lexleo/test/fake_logger_adapter.h"

extern logger_dynamic_buffer_creator_t *g_fake_logger_dynamic_buffer_creator;
extern logger_regular_file_creator_t *g_fake_logger_regular_file_creator;
extern logger_standard_logger_creator_t *g_fake_logger_standard_logger_creator;

typedef logger_t fake_logger_t;

fake_logger_t *fake_logger_create_instance(fake_logger_adapter_t *fake_logger_adapter);

void fake_logger_reset_instance(fake_logger_t *fake_logger);
void fake_logger_reset(void);

#endif /* LEXLEO_FAKE_LOGGER_H */
