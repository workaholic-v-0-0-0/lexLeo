/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_memory.h
 * @ingroup test_support_fake_memory
 * @brief Fake memory backend for unit tests.
 *
 * @details
 * This header declares fake memory operations and helper functions for
 * configuring and observing fake memory behavior in tests.
 *
 * Tests can allocate memory from a fixed arena, inject allocation failures at
 * configured call indices, reset fake state, and detect leaks, invalid frees,
 * and double frees.
 */

#ifndef LEXLEO_FAKE_MEMORY_H
#define LEXLEO_FAKE_MEMORY_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FAKE API */

void *fake_malloc(size_t size);
void fake_free(void *ptr);
void *fake_calloc(size_t nmemb, size_t size);
void *fake_realloc(void *ptr, size_t size);
void *fake_memcpy(void *dst, const void *src, size_t n);
void *fake_memset(void *dst, int c, size_t n);

/* CFG */

void fake_memory_reset(void);
void fake_memory_fail_on_calls(size_t n, const size_t *idxs);
void fake_memory_fail_only_on_call(size_t n);
void fake_memory_fail_on_all_call(void);
void fake_memory_fail_since(size_t n);

/* SPY */

bool fake_memory_no_leak(void);
bool fake_memory_no_invalid_free(void);
bool fake_memory_no_double_free(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_FAKE_MEMORY_H
