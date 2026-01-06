/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "<module>_test_api.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

//-----------------------------------------------------------------------------
// TEST
// <fct prototype under test>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TEST CONTEXT
//-----------------------------------------------------------------------------
// Isolation:
//   <list not doubled dependencies>
//
// Test doubles:
//   <list doubled dependencies>
//

//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
	const char *name;

	// arrange

	// assert

	// test infrastructure cleanup

	// information sharing

} test_<module>_<fct>_case_t;

typedef struct {


	const test_lexleo_flex_destroy_case_t *tc;
} test_<module>_<fct>_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_<fct>(void **state) {


	return 0;
}

static int teardown_<fct>(void **state) {

	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_<fct>(void **state) {
	const test_<module>_<fct>_case_t *test_case =
		(const test_<module>_<fct>_case_t *) *state;

}




















// tests/test_support/templates/unit_test_template.c

