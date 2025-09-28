// src/data_structures/tests/test_list_real_malloc.c

// TODO FROM SCRATCH

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

typedef unsigned char boolean;
static const boolean TRUE = 1;
static const boolean FALSE = 0;

static list l;
#define EMPTY_LIST NULL
static size_t list_len;
static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';


static char dummy;
static const list LIST_DEFINED_IN_SETUP = (list) &dummy;
static void *const ELEMENT_DEFINED_IN_SETUP = (void *) &dummy;



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES
//-----------------------------------------------------------------------------

static char dummy; // ???



//-----------------------------------------------------------------------------
// GENERALS HELPERS
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


typedef struct {
    const char *label;
    boolean chars_are_dynamically_allocated;
	char **char_ptrs;
    list l;
    void *e;
} list_push_test_params_t;



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static list_push_test_params_t lNull_eNull = {
    .label = "l == NULL, e == NULL",
    .chars_are_dynamically_allocated = FALSE,
    .l = NULL,
    .e = NULL,
};

static list_push_test_params_t lNotNull_eNull_staticAllocation = {
    .label = "l != NULL, e == NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = NULL,
};

static list_push_test_params_t lNotNull_eNull_dynamicAllocation = {
    .label = "l != NULL, e == NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = NULL,
};

static list_push_test_params_t lNull_eNotNull_staticAllocation = {
    .label = "l == NULL, e != NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .l = NULL,
    .e = ELEMENT_DEFINED_IN_SETUP,
};

static list_push_test_params_t lNull_eNotNull_dynamicAllocation = {
    .label = "l == NULL, e != NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .l = NULL,
    .e = ELEMENT_DEFINED_IN_SETUP,
};

static list_push_test_params_t lNotNull_eNotNull_staticAllocation = {
    .label = "l != NULL, e != NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = ELEMENT_DEFINED_IN_SETUP,
};

static list_push_test_params_t lNotNull_eNotNull_dynamicAllocation = {
    .label = "l != NULL, e != NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = ELEMENT_DEFINED_IN_SETUP,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_push_setup(void **state) {
    list_push_test_params_t *params = (list_push_test_params_t *) (*state);
    if (params->l == LIST_DEFINED_IN_SETUP) {
        list_len = 2;
        if (params->chars_are_dynamically_allocated) {
            params->char_ptrs = create_dynamic_char_ptr_array_n(list_len, 'A', 'B');
        }
        else {
            params->char_ptrs = create_static_char_ptr_array_n(list_len, &STATIC_CHAR_A, &STATIC_CHAR_B);
        }
        assert_non_null(params->char_ptrs);
        params->l = make_list_n((void **) params->char_ptrs, list_len);
    }
    if (params->e == ELEMENT_DEFINED_IN_SETUP) {
        if (params->chars_are_dynamically_allocated) {
            params->e = malloc(sizeof(char));
            assert_non_null(params->e);
        }
    }
    return 0;
}

static int list_push_teardown(void **state) {
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    if (params->l == LIST_DEFINED_IN_SETUP) {
        list next = NULL;
        while (params->l) {
            next = (params->l)->cdr;
            free(l);
            params->l = next;
        }
        if (params->chars_are_dynamically_allocated)
            destroy_dynamic_char_ptr_array_n(params->char_ptrs, list_len);
        else
            free(params->char_ptrs);
    }
/*
    if (params->e == ELEMENT_DEFINED_IN_SETUP)
        free(params->e);
*/
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Note:
// This quasi-unit test (integrating standard library).
// Assume all malloc call success.

/* tests list
cases :
e == address of a static char, l == NULL
e == address of a dynamic char, l == NULL
e == address of a static char, l == a list with two elements
e == address of a dynamic char, l == a list with two elements

with wrong params, memory does not change (l does not change, e does not change)
with right params, e does not change, l has right value
*/

// Given: e == NULL, l == a list with two elements, static allocation
// Expected: *l is not modified
// Param:
//    - lNotNull_eNull_staticAllocation
//    - lNotNull_eNull_dynamicAllocation
/*
static void list_push_lIsNotModified_WhenLNotNull_AndENull(void **state) {
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    cons cons_before = *(params->l);
    list_push(params->l, params->e);
    assert_memory_equal(params->l, &cons_before, sizeof(cons));
}
*/

// e does not change

// Given: e != 0, l == a list with two elements, static allocation
// Expected: *l is
// Param:
//    - lNull_eNotNull_staticAllocation
//    - lNull_eNotNull_dynamicAllocation
//    - lNotNull_eNotNull_staticAllocation
//    - lNotNull_eNotNull_dynamicAllocation
/*
static void list_push_lIsCorrectlyPrepended_WhenLNotNull_AndENotNull(void **state) {
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    cons cons_before;
    cons_before.car = (params->l)->car;
    cons_before.cdr = (params->l)->cdr;
    list_push(params->l, params->e);
    assert_memory_equal((params->l)->cdr, &cons_before, sizeof(cons));
}
*/


















/*

// Given: e == address of a static char, l == NULL
// Expected: list_push returns a valid initialized list pointer
// Param:
static void list_push_ReturnsAValidPointer_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(NULL, &INT_1);
    assert_non_null(ret);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_1_ELEMENT, malloc success
// Expected: list_push returns a valid initialized list pointer
static void list_push_ReturnsAValidPointer_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_1_ELEMENT, &INT_1);
    assert_non_null(ret);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by l does not change
static void list_push_NoChangeValueAtL_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    cons save = *A_LIST_WITH_2_ELEMENTS;
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_memory_equal(A_LIST_WITH_2_ELEMENTS, &save, sizeof(cons));
    free(ret);
}

// Given: e == &INT_1, l == NULL
// Expected : the value pointed by e does not change
static void list_push_NoChangeValueAtE_WhenLNull_AndENotNull(void **state) {
    (void) *state; // unused
    int save = INT_1;
    list ret = list_push(NULL, &INT_1);
    assert_int_equal(INT_1, save);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : the value pointed by e does not change
static void list_push_NoChangeValueAtE_WhenLNotNull_AndENotNull(void **state) {
    (void) *state; // unused
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_int_equal(INT_1, * (int *) &INT_1);
    free(ret);
}

// Given: e == &INT_1, l == NULL
// Expected : ret->car == &INT_1, ret->cdr == NULL
static void list_push_ReturnConsWithRightValues_WhenLNull_AndENotNull(void **state) {
    (void) *state;  // unused
    list ret = list_push(NULL, &INT_1);
    assert_ptr_equal(ret->car, &INT_1);
    assert_ptr_equal(ret->cdr, NULL);
    free(ret);
}

// Given: e == &INT_1, l == A_LIST_WITH_2_ELEMENTS
// Expected : ret->car == &INT_1, ret->cdr == NULL
static void list_push_ReturnConsWithRightValues_WhenLNotNull_AndENotNull(void **state) {
    (void) *state;  // unused
    list ret = list_push(A_LIST_WITH_2_ELEMENTS, &INT_1);
    assert_ptr_equal(ret->car, &INT_1);
    assert_ptr_equal(ret->cdr, A_LIST_WITH_2_ELEMENTS);
    free(ret);
}

*/



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest list_push_tests[] = {
/*
        cmocka_unit_test_prestate_setup_teardown(
            list_push_lIsNotModified_WhenLNotNull_AndENull,
            list_push_setup, list_push_teardown, &lNotNull_eNull_staticAllocation),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_lIsNotModified_WhenLNotNull_AndENull,
            list_push_setup, list_push_teardown, &lNotNull_eNull_dynamicAllocation),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_lIsCorrectlyPrepended_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown, &lNotNull_eNotNull_staticAllocation),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_lIsCorrectlyPrepended_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown, &lNotNull_eNotNull_dynamicAllocation),
*/
/*        cmocka_unit_test_setup_teardown(
            list_push_ReturnsAValidPointer_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_ReturnsAValidPointer_WhenLNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_ReturnsAValidPointer_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_NoChangeValueAtL_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_NoChangeValueAtE_WhenLNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_NoChangeValueAtE_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_ReturnConsWithRightValues_WhenLNull_AndENotNull,
            list_push_setup, list_push_teardown),
        cmocka_unit_test_setup_teardown(
            list_push_ReturnConsWithRightValues_WhenLNotNull_AndENotNull,
            list_push_setup, list_push_teardown),
*/
    };
    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    return failed;
}
