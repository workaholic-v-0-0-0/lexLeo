// src/data_structures/tests/test_list.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

#include "logger.h"

#include "list.h"



//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------


static list l;
static size_t list_length;
static char **ptrs_on_static_chars;
static char **ptrs_on_dynamic_chars;
static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';

#define EMPTY_LIST NULL


/*
static int INT_1 = 1;
static int INT_2 = 2;
static cons A_CONS_WITH_1_ELEMENT = {
    (void *) &INT_1,
    NULL
};
static list A_LIST_WITH_1_ELEMENT = &A_CONS_WITH_1_ELEMENT;
static list A_LIST_WITH_2_ELEMENTS; // must be initialized in setup
*/




//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------

static char dummy;
static char *dummy_ptr; // must be initialized in setup

void * list_malloc(size_t size) {
    check_expected_ptr(size);
    return mock_type(void *);
}

void list_free(void *ptr) {
    check_expected_ptr(ptr);
}

void dummy_destroy(void *ptr) {
    check_expected_ptr(ptr);
}



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------

list make_list_n(void **ptrs, size_t n) {
    list head = NULL, prev = NULL;
    for (size_t i = 0; i < n; ++i) {
        list node = malloc(sizeof(cons));
        assert_non_null(node);
        node->car = ptrs[i];
        node->cdr = NULL;
        if (!head) head = node;
        if (prev) prev->cdr = node;
        prev = node;
    }
    return head;
}

// optionals are char
char **create_dynamic_char_ptr_array_n(size_t size,...) {
    va_list args;
    va_start(args, size);
    char **ptrs = malloc(sizeof(char *) * size);
    assert_non_null(ptrs);
    for (size_t i = 0; i < size; ++i) {
        ptrs[i] = malloc(sizeof(char));
        assert_non_null(ptrs[i]);
        * ptrs[i] = (char) va_arg(args, int);
    }
    va_end(args);
    return ptrs;
}

void destroy_dynamic_char_ptr_array_n(char ** ptrs, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(ptrs[i]);
    }
    free(ptrs);
}

// optionals are char * on statically allocated octet
char **create_static_char_ptr_array_n(size_t size,...) {
    va_list args;
    va_start(args, size);
    char **ptrs = malloc(sizeof(char *) * size);
    assert_non_null(ptrs);
    for (size_t i = 0; i < size; ++i) {
        ptrs[i] = va_arg(args, char *);
    }
    va_end(args);
    return ptrs;
}



//-----------------------------------------------------------------------------
// list_push TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


// HERE NOW



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


// HERE NOW



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_push_setup(void **state) {
    (void) *state;  // unused
    list_length = 3;
    ptrs_on_static_chars =
        create_static_char_ptr_array_n(
            list_length,
            &STATIC_CHAR_A,
            &STATIC_CHAR_B,
            &STATIC_CHAR_C);
    l = make_list_n((void **) ptrs_on_static_chars, list_length);
    return 0;
}

static int list_push_teardown(void **state) {
    (void) *state;
    list next = NULL;
    while (l) {
        next = l->cdr;
        free(l);
        l = next;
    }
    free(ptrs_on_static_chars);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


/*

// Given: e == NULL, l == NULL
// Expected : list_push returns NULL
static void list_push_Error_WhenLNull_AndENull(void **state) {
    (void) *state; // unused
    list ret = list_push(NULL, NULL);
    assert_null(ret);
}

// Given: e == NULL, l == A_LIST_WITH_1_ELEMENT
// Expected : list_push returns NULL
static void list_push_Error_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_1_ELEMENT, NULL);
    assert_null(ret);
}

// Given: e == NULL, l == NULL
// Expected : malloc is not called
static void list_push_DoNotCallMalloc_WhenLNull_AndENull(void **state) {
    (void) *state; // unused
    list_push(NULL, NULL);
}

// Given: e == NULL, l == A_LIST_WITH_1_ELEMENT
// Expected : malloc is not called
static void list_push_DoNotCallMalloc_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    list_push(A_LIST_WITH_1_ELEMENT, NULL);
}

// Given: e == &INT_1, l == NULL
// Expected : malloc is called with sizeof(cons)
static void list_push_CallMalloc_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, &dummy);
    list_push(NULL, &INT_1);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT
// Expected : malloc is called with sizeof(cons)
static void list_push_CallMalloc_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, &dummy);
    list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT, malloc fail
// Expected : ret is NULL
static void list_push_Error_WhenMallocFail(void **state) {
    (void) *state; // unused
    expect_value(list_malloc, size, sizeof(cons));
    will_return(list_malloc, NULL);
    list ret = list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
    assert_int_equal(ret, NULL);
}

// Given: e == NULL, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by l does not change
static void list_push_NoChangeValueAtL_WhenLNotNull_AndENull(void **state) {
    (void) *state; // unused
    cons save = *A_LIST_WITH_2_ELEMENTS;
    list_push(A_LIST_WITH_2_ELEMENTS, NULL);
    assert_memory_equal(A_LIST_WITH_2_ELEMENTS, &save, sizeof(cons));
}

*/



//-----------------------------------------------------------------------------
// list_free TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int list_free_list_setup(void **state) {
    (void) *state;  // unused
    list_length = 3;
    ptrs_on_static_chars =
        create_static_char_ptr_array_n(
            list_length,
            &STATIC_CHAR_A,
            &STATIC_CHAR_B,
            &STATIC_CHAR_C);
    l = make_list_n((void **) ptrs_on_static_chars, list_length);
    return 0;
}

static int list_free_list_teardown(void **state) {
    (void) *state;
    list next = NULL;
    while (l) {
        next = l->cdr;
        free(l);
        l = next;
    }
    free(ptrs_on_static_chars);
    return 0;
}

/*
static int list_free_list_setup(void **state) {
    (void) *state;  // unused
    INT_1 = 1;
    dummy_ptr = malloc(sizeof(char));
    A_LIST_WITH_2_ELEMENTS = malloc(sizeof(cons));
    assert_non_null(A_LIST_WITH_2_ELEMENTS);
    A_LIST_WITH_2_ELEMENTS->car = &dummy_ptr;
    A_LIST_WITH_2_ELEMENTS->cdr = (list) &A_LIST_WITH_1_ELEMENT;
    return 0;
}

static int list_free_list_teardown(void **state) {
    free(A_LIST_WITH_2_ELEMENTS);
    free(dummy_ptr);
    return 0;
}
*/



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------




/*

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == NULL
// Expected : list_free is called with l
static void list_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyNull(void **state) {
    (void) *state; // unused
    expect_value(list_free, ptr, A_LIST_WITH_2_ELEMENTS);
    list_free_list(A_LIST_WITH_2_ELEMENTS, NULL);
}

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == dummy_destroy
// Expected : list_free is called with l
static void list_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy(void **state) {
    (void) *state; // unused
    expect_value(list_free, ptr, A_LIST_WITH_2_ELEMENTS);
    expect_value(dummy_destroy, ptr, A_LIST_WITH_2_ELEMENTS->car);
    list_free_list(A_LIST_WITH_2_ELEMENTS, dummy_destroy);
}

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == dummy_destroy
// Expected : dummy_destroy is called with l->car
static void list_free_list_Call_dummy_destroy_WithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy(void **state) {
    (void) *state; // unused
    expect_value(list_free, ptr, A_LIST_WITH_2_ELEMENTS);
    expect_value(dummy_destroy, ptr, A_LIST_WITH_2_ELEMENTS->car);
    list_free_list(A_LIST_WITH_2_ELEMENTS, dummy_destroy);
}

*/



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest list_push_tests[] = {

/*
        cmocka_unit_test(
            list_push_Error_WhenLNull_AndENull),
        cmocka_unit_test_setup_teardown(
            list_push_Error_WhenLNotNull_AndENull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_DoNotCallMalloc_WhenLNull_AndENull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_DoNotCallMalloc_WhenLNotNull_AndENull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_CallMalloc_WhenLNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_CallMalloc_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_Error_WhenMallocFail,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_NoChangeValueAtL_WhenLNotNull_AndENull,
            list_push_setup, list_push_teardown),
*/
    };

    const struct CMUnitTest list_free_list_tests[] = {

/*
        cmocka_unit_test_setup_teardown(
            list_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyNull,
            list_free_list_setup, list_free_list_teardown),
        cmocka_unit_test_setup_teardown(
            list_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy,
            list_free_list_setup, list_free_list_teardown),
        cmocka_unit_test_setup_teardown(
            list_free_list_Call_dummy_destroy_WithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy,
            list_free_list_setup, list_free_list_teardown),
*/
    };

    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    failed += cmocka_run_group_tests(list_free_list_tests, NULL, NULL);
    return failed;
}
