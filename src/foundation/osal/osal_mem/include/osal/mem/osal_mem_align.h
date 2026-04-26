/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem_align.h
 * @ingroup osal_mem_api
 * @brief Alignment helpers
 *
 * @details
 * These helpers abstract compiler-specific alignment keywords and expose
 * common alignment calculations through a stable interface.
 */

#ifndef LEXLEO_OSAL_MEM_ALIGN_H
#define LEXLEO_OSAL_MEM_ALIGN_H

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Maximum alignment used by portable OSAL memory helpers.
 *
 * @details
 * When C11 alignment support is available, this macro expands to
 * `alignof(max_align_t)`.
 *
 * Otherwise, it falls back to `16`, which is used as a conservative alignment
 * value for supported targets.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
	#include <stdalign.h>
	#define OSAL_ALIGNOF_MAX alignof(max_align_t)

/**
 * @brief Declaration specifier for objects aligned to `OSAL_ALIGNOF_MAX`.
 *
 * @details
 * This macro abstracts C11 alignment syntax and expands to
 * `_Alignas(max_align_t)`.
 *
 * Example:
 *     OSAL_ALIGNED_MAX static uint8_t buffer[1024];
 */
	#define OSAL_ALIGNED_MAX _Alignas(max_align_t)
#else
	#if defined(_MSC_VER)
		#define OSAL_ALIGNOF_MAX 16

/**
 * @brief Declaration specifier for objects aligned to `OSAL_ALIGNOF_MAX`.
 *
 * @details
 * This macro abstracts MSVC alignment syntax and expands to
 * `__declspec(align(16))`.
 *
 * Example:
 *
 *     OSAL_ALIGNED_MAX static uint8_t buffer[1024];
 */
		#define OSAL_ALIGNED_MAX __declspec(align(16))
	#else
		#define OSAL_ALIGNOF_MAX 16

/**
 * @brief Declaration specifier for objects aligned to `OSAL_ALIGNOF_MAX`.
 *
 * @details
 * This macro abstracts GCC/Clang alignment syntax and expands to
 * `__attribute__((aligned(16)))`.
 *
 * Example:
 *
 *     OSAL_ALIGNED_MAX static uint8_t buffer[1024];
 */
		#define OSAL_ALIGNED_MAX __attribute__((aligned(16)))
	#endif
#endif

/**
 * @brief Round a value up to the next alignment boundary.
 *
 * @param x Value to align.
 * @param a Alignment value. Must be a power of two.
 *
 * @return The smallest multiple of `a` greater than or equal to `x`.
 *
 * @warning `a` must be a power of two.
 * @warning The expression may evaluate its arguments more than once.
 */
#ifndef OSAL_ALIGN_UP
	#define OSAL_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#endif

#endif /* LEXLEO_OSAL_MEM_ALIGN_H */
