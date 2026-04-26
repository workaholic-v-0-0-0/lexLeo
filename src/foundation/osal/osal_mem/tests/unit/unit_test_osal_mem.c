/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_mem.c
 * @ingroup osal_mem_unit_tests
 * @brief Unit tests for the public `osal_mem` module.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_mem_default_ops()`
 * - `osal_malloc()`
 * - `osal_calloc()`
 * - `osal_realloc()`
 * - `osal_free()`
 * - `osal_memcpy()`
 * - `osal_memset()`
 * - `osal_memmove()`
 */

#if !defined(__unix__) && !defined(__APPLE__)
#error "These osal_mem unit tests are supported only on POSIX-like platforms."
#endif

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `osal_mem_default_ops()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_default_ops "osal_mem_default_ops() unit tests section"
 * - @ref specifications_osal_mem_default_ops "osal_mem_default_ops() specifications".
 */
static void test_osal_mem_default_ops(void **state) {
	(void)state;

	const osal_mem_ops_t *ret = osal_mem_default_ops();

	assert_non_null(ret);
	assert_non_null(ret->malloc);
	assert_non_null(ret->free);
	assert_non_null(ret->calloc);
	assert_non_null(ret->realloc);
	assert_ptr_equal(ret->malloc, osal_malloc);
	assert_ptr_equal(ret->free, osal_free);
	assert_ptr_equal(ret->calloc, osal_calloc);
	assert_ptr_equal(ret->realloc, osal_realloc);
}

/**
 * @brief Test `osal_malloc()` and `osal_free()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_malloc_osal_free "osal_malloc() and osal_free() unit tests section"
 * - @ref specifications_osal_malloc "osal_malloc() specifications".
 * - @ref specifications_osal_free "osal_free() specifications".
 */
static void test_osal_mem_osal_malloc_osal_free(void **state) {
	(void)state;
	uint8_t *p = NULL;
	p = osal_malloc(4);
	assert_non_null(p);
	*p = 0x00;
	*(p+1) = 0x01;
	*(p+2) = 0x02;
	*(p+3) = 0x03;
	osal_free(p);
}

/**
 * @brief Test `osal_calloc()` and `osal_free()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_calloc_osal_free "osal_calloc() and osal_free() unit tests section"
 * - @ref specifications_osal_calloc "osal_calloc() specifications".
 * - @ref specifications_osal_free "osal_free() specifications".
 */
static void test_osal_mem_osal_calloc_osal_free(void **state) {
	(void)state;
	uint16_t *p = NULL;
	p = osal_calloc(3,sizeof(uint16_t));
	assert_non_null(p);
	uint16_t zero16 = (uint16_t)0x0000;
	for (int i = 0; i < 3; i++) {
		assert_memory_equal(p+i, &zero16, sizeof(uint16_t));
	}
	*p = (uint16_t)0x0000;
	*(p+1) = (uint16_t)0x0001;
	*(p+2) = (uint16_t)0x0100;
	osal_free(p);
}

/**
 * @brief Test `osal_realloc()` and `osal_free()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_realloc_osal_free "osal_realloc() and osal_free() unit tests section"
 * - @ref specifications_osal_realloc "osal_realloc() specifications".
 * - @ref specifications_osal_free "osal_free() specifications".
 */
static void test_osal_mem_osal_realloc_osal_free(void **state) {
	(void)state;
	// arrange
	uint16_t *p = NULL;
	uint16_t *ret = NULL;
	uint16_t value16 = (uint16_t)0XABCD;
	p = osal_malloc(sizeof(uint16_t));
	assert_non_null(p);
	*p = (uint16_t)value16;

	// act
	ret = osal_realloc(p, sizeof(uint32_t));

	// assert
	assert_non_null(ret);
	assert_memory_equal(ret, &value16, sizeof(uint16_t));

	// teardown
	osal_free(ret);
}

/**
 * @brief Test `osal_memcpy()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_memcpy "osal_memcpy() unit tests section"
 * - @ref specifications_osal_memcpy "osal_memcpy() specifications".
 */
static void test_osal_mem_osal_memcpy(void **state) {
	(void)state;

	// arrange
	uint16_t *dest = NULL;
	uint16_t *src = NULL;
	void *ret = NULL;
	uint16_t zero16 = (uint16_t)0x0000;
	uint16_t value16 = (uint16_t)0XABCD;
	dest = osal_malloc(sizeof(uint16_t));
	src = osal_malloc(sizeof(uint16_t));
	assert_non_null(dest);
	assert_non_null(src);
	*src = (uint16_t)value16;
	*dest = (uint16_t)zero16;

	// act
	ret = osal_memcpy(dest, src, sizeof(uint16_t));

	// assert
	assert_non_null(ret);
	assert_ptr_equal(ret, dest);
	assert_memory_equal(ret, &value16, sizeof(uint16_t));

	// teardown
	osal_free(dest);
	osal_free(src);
}

/**
 * @brief Test `osal_memset()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_memset "osal_memset() unit tests section"
 * - @ref specifications_osal_memset "osal_memset() specifications".
 */
static void test_osal_mem_osal_memset(void **state) {
	(void)state;

	// arrange
	void *ret = NULL;
	uint32_t zero32 = (uint32_t)0x00000000;
	uint8_t value8 = (uint8_t)0XAB;
	uint8_t *p = osal_calloc(4, sizeof(uint8_t));
	assert_non_null(p);
	assert_memory_equal(p, &zero32, sizeof(uint32_t));

	// act
	ret = osal_memset(p, value8, sizeof(uint32_t));

	// assert
	assert_non_null(ret);
	assert_ptr_equal(ret, p);
	for (int i = 0; i < 4; i++) {
		assert_memory_equal(p+i, &value8, sizeof(uint8_t));
	}

	// teardown
	osal_free(p);
}

/**
 * @brief Test `osal_memmove()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_mem_unit_osal_memmove "osal_memmove() unit tests section"
 * - @ref specifications_osal_memmove "osal_memmove() specifications".
 */
static void test_osal_mem_osal_memmove(void **state) {
	(void)state;

	// arrange
	void *ret = NULL;
	uint32_t zero32 = (uint32_t)0x00000000;
	uint8_t value_0XAB = (uint8_t)0XAB;
	uint8_t value_0XCD = (uint8_t)0XCD;
	uint8_t *store32 = osal_calloc(4, sizeof(uint8_t));
	assert_non_null(store32);
	assert_memory_equal(store32, &zero32, sizeof(uint32_t));
	assert_non_null(osal_memcpy(store32 + 1, &value_0XAB, sizeof(uint8_t)));
	assert_memory_equal(store32 + 1, &value_0XAB, sizeof(uint8_t));
	assert_non_null(osal_memcpy(store32 + 2, &value_0XCD, sizeof(uint8_t)));
	assert_memory_equal(store32 + 2, &value_0XCD, sizeof(uint8_t));
	uint8_t *dest = store32;
	uint8_t *src = store32 + 1;

	// act
	ret = osal_memmove(dest, src, 2 * sizeof(uint8_t));

	// assert
	assert_non_null(ret);
	assert_ptr_equal(ret, dest);
	assert_memory_equal(store32, &value_0XAB, sizeof(uint8_t));
	assert_memory_equal(store32 + 1, &value_0XCD, sizeof(uint8_t));
	assert_memory_equal(store32 + 2, &value_0XCD, sizeof(uint8_t));

	// teardown
	osal_free(store32);
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest osal_mem_non_parametric_tests[] = {
		cmocka_unit_test(test_osal_mem_default_ops),
		cmocka_unit_test(test_osal_mem_osal_malloc_osal_free),
		cmocka_unit_test(test_osal_mem_osal_calloc_osal_free),
		cmocka_unit_test(test_osal_mem_osal_realloc_osal_free),
		cmocka_unit_test(test_osal_mem_osal_memcpy),
		cmocka_unit_test(test_osal_mem_osal_memset),
		cmocka_unit_test(test_osal_mem_osal_memmove)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(osal_mem_non_parametric_tests, NULL, NULL);

	return failed;
}

/** @endcond */
