/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_PANIC_H
#define LEXLEO_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

void lexleo_panic(const char *msg);
void lexleo_panic_oom(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_PANIC_H