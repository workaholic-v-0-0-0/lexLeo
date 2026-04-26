/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_env.h
 * @ingroup osal_env_api
 * @brief Public API of the `osal_env` module.
 *
 * @details
 * This header declares the portable environment access services provided by
 * the Operating System Abstraction Layer.
 *
 * Current public services:
 * - `osal_getenv()`
 */

#ifndef LEXLEO_OSAL_ENV_H
#define LEXLEO_OSAL_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

const char *osal_getenv(const char *name);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_ENV_H
