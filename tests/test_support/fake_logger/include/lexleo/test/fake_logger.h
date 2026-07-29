/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_logger.h
 * @ingroup test_support_fake_logger
 * @brief Fake logger support for unit tests.
 *
 * @details
 * Creates a `logger_t` backed by a caller-provided fake adapter.
 * The reset function clears the adapter's configuration and observations.
 */

#ifndef LEXLEO_FAKE_LOGGER_H
#define LEXLEO_FAKE_LOGGER_H

#include "logger/common/logger_opaque_type.h"

#include "lexleo/test/fake_logger_adapter.h"

typedef logger_t fake_logger_t;

fake_logger_t *fake_logger_create_instance(fake_logger_adapter_t *fake_logger_adapter);

void fake_logger_reset();
void fake_logger_reset_instance(fake_logger_t *fake_logger);

#endif /* LEXLEO_FAKE_LOGGER_H */
