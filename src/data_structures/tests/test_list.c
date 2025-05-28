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

void * list_malloc(size_t size) {
    check_expected_ptr(size);
    return mock_type(void *);
}



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


// Given: e == NULL, l == NULL
// Expected : push_list returns NULL
static void push_list_Error_WhenLNull_AndENull(void **state) {
    (void) *state; // unused
    list ret = list_push(NULL, NULL);
    assert_null(ret);
}

// Given: e == NULL, l == A_LIST_WITH_1_ELEMENT
// Expected : push_list returns NULL
static void push_list_Error_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_1_ELEMENT, NULL);
    assert_null(ret);
}

// Given: e == NULL, l == NULL
// Expected : malloc is not called
static void push_list_DoNotCallMalloc_WhenLNull_AndENull(void **state) {
    (void) *state; // unused
    list_push(NULL, NULL);
}

// Given: e == NULL, l == A_LIST_WITH_1_ELEMENT
// Expected : malloc is not called
static void push_list_DoNotCallMalloc_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    list_push(A_LIST_WITH_1_ELEMENT, NULL);
}

// Given: e == &INT_1, l == NULL
// Expected : malloc is called with sizeof(cons)
static void push_list_CallMalloc_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, &dummy);
    list_push(NULL, &INT_1);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT
// Expected : malloc is called with sizeof(cons)
static void push_list_CallMalloc_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, &dummy);
    list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT, malloc fail
// Expected : ret is NULL
static void push_list_Error_WhenMallocFail(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, NULL);
    list ret = list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
    assert_int_equal(ret, NULL);
}

// Given: e == NULL, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by l does not change
static void push_list_NoChangeValueAtL_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    cons save = *A_LIST_WITH_2_ELEMENTS;
    list_push(A_LIST_WITH_2_ELEMENTS, NULL);
    assert_memory_equal(A_LIST_WITH_2_ELEMENTS, &save, sizeof(cons));
}



//-----------------------------------------------------------------------------
// list_free TESTS
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest list_push_tests[] = {
        cmocka_unit_test(
            push_list_Error_WhenLNull_AndENull),
        cmocka_unit_test_setup_teardown(
            push_list_Error_WhenLNotNull_AndENull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_DoNotCallMalloc_WhenLNull_AndENull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_DoNotCallMalloc_WhenLNotNull_AndENull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_CallMalloc_WhenLNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_CallMalloc_WhenLNotNull_AndENotNull,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_Error_WhenMallocFail,
            push_list_setup, push_list_teardown),
        cmocka_unit_test_setup_teardown(
            push_list_NoChangeValueAtL_WhenLNotNull_AndENull,
            push_list_setup, push_list_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    return failed;
}
