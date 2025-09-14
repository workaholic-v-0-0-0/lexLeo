// src/interpreter/tests/test_interpreter.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "interpreter.h"
#include "fake_memory.h"


//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES, MAGIC NUMBER KILLERS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// dummies
// mocks
// spies
// stubs


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core:
    interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements:
 -
*/

// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_atom_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int eval_atom_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test:
// Given:
//  -
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  -
// Expected:
//  -
static void eval_atom__when_(void **state) {
    (void)state;
    // runtime_env and ast

    eval_atom(bbb);
}





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
/*
    const struct CMUnitTest _tests[] = {
    };
*/

/*
    int failed = 0;
    failed += cmocka_run_group_tests(, NULL, NULL);
*/

    //return failed;
    return 0;
}
