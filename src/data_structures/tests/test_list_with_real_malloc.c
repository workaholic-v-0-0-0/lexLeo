// src/data_structures/tests/test_list.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

#include "logger.h"

#include "list.h"



//-----------------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------------


#define EMPTY_LIST NULL
static int INT_1 = 1;
static int INT_2 = 2;
static cons A_CONS_WITH_1_ELEMENT = {
    (void *) &INT_1,
    NULL
};
static list A_LIST_WITH_1_ELEMENT = &A_CONS_WITH_1_ELEMENT;
static list A_LIST_WITH_2_ELEMENTS; // must be initialized in setup



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES
//-----------------------------------------------------------------------------

static char dummy;



//-----------------------------------------------------------------------------
// push_list TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int push_list_setup(void **state) {
    (void) *state;  // unused
    INT_1 = 1;
    INT_2 = 2;
    A_LIST_WITH_2_ELEMENTS = malloc(sizeof(cons));
    assert_non_null(A_LIST_WITH_2_ELEMENTS);
    A_LIST_WITH_2_ELEMENTS->car = &INT_2;
    A_LIST_WITH_2_ELEMENTS->cdr = (list) &A_LIST_WITH_1_ELEMENT;
    return 0;
}

static int push_list_teardown(void **state) {
    free(A_LIST_WITH_2_ELEMENTS);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: e == &INT_1, l == NULL, malloc success
// Expected: push_list returns a valid initialized list pointer
static void push_list_ReturnsAValidPointer_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(NULL, &INT_1);
    assert_non_null(ret);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT, malloc success
// Expected: push_list returns a valid initialized list pointer
static void push_list_ReturnsAValidPointer_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
    assert_non_null(ret);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by l does not change
static void push_list_NoChangeValueAtL_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    cons save = *A_LIST_WITH_2_ELEMENTS;
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_memory_equal(A_LIST_WITH_2_ELEMENTS, &save, sizeof(cons));
    free(ret);
}

// Given: e == &INT_1, l == NULL
// Expected : the value pointed by e does not change
static void push_list_NoChangeValueAtE_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    int save = INT_1;
    list ret = list_push(NULL, &INT_1);
    assert_int_equal(INT_1, save);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by e does not change
static void push_list_NoChangeValueAtE_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_int_equal(INT_1, * (int *) &INT_1);
    free(ret);
}

// Given: e == &INT_1, l == NULL
// Expected : ret->car == &INT_1, ret->cdr == NULL
static void push_list_ReturnConsWithRightValues_WhenLNull_AndENotNull(void **state) {
    (void) *state;  // unused
    list ret = list_push(NULL, &INT_1);
    assert_ptr_equal(ret->car, &INT_1);
    assert_ptr_equal(ret->cdr, NULL);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : ret->car == &INT_1, ret->cdr == NULL
static void push_list_ReturnConsWithRightValues_WhenLNotNull_AndENotNull(void **state) {
    (void) *state;  // unused
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_ptr_equal(ret->car, &INT_1);
    assert_ptr_equal(ret->cdr, A_LIST_WITH_2_ELEMENTS);
    free(ret);
}





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest list_push_tests[] = {
        cmocka_unit_test_setup_teardown(
            push_list_ReturnsAValidPointer_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_ReturnsAValidPointer_WhenLNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_ReturnsAValidPointer_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_NoChangeValueAtL_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_NoChangeValueAtE_WhenLNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_NoChangeValueAtE_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_ReturnConsWithRightValues_WhenLNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_ReturnConsWithRightValues_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    return failed;
}
