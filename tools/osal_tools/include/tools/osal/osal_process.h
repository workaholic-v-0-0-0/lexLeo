/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_OSAL_PROCESS_H
#define LEXLEO_OSAL_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

int osal_spawn(const char *program, char *const argv[]);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_PROCESS_H
