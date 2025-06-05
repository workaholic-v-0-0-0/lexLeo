// src/data_structures/tests/test_hashtable.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "memory_allocator.h"
#include "logger.h"
#include "list.h"

#include "internal/hashtable_internal.h"



//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------


typedef unsigned char boolean;
#define TRUE 1
#define FALSE 0
#define DUMMY_BOOLEAN_VALUE 0

static const char dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
static const void *DUMMY_FREE_RETURNED_VALUE = (void *) &dummy;
static const hashtable_destroy_value_fn_t DUMMY_HASHTABLE_DESTROY_VALUE_FN = (hashtable_destroy_value_fn_t) &dummy;
#define MALLOC_ERROR_CODE NULL

static const int DUMMY_INT_VALUE = 0;
static void *const DUMMY_POINTER_VALUE = NULL;
static const size_t DUMMY_SIZE_T_VALUE = 0;
static const hashtable * HASHTABLE_DEFINED_IN_SETUP = (hashtable *) &dummy;
static const entry * ENTRIES_DEFINED_IN_SETUP = (entry *) &dummy;

static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';
static char STATIC_CHARS[3] = {STATIC_CHAR_A, STATIC_CHAR_B, STATIC_CHAR_C};
static const char * const KEY_VALUES[3] = {"key_for_A", "key_for_B", "key_for_C"};



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_hashtable;
static void *fake_malloc_returned_value_for_buckets;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}



//-----------------------------------------------------------------------------
// hashtable_create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    size_t s;
    hashtable_destroy_value_fn_t f;
} create_test_params_t;



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static size_t param_s(void **state) {
    return ((create_test_params_t *) *state)->s;
}

static hashtable_destroy_value_fn_t param_f(void **state) {
    return ((create_test_params_t *) *state)->f;
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static create_test_params_t create_s_0_f_null = {
    .label = "s == 0, f == NULL",
    .s = 0,
    .f = NULL,
};

static create_test_params_t create_s_0_f_dummy_not_null = {
    .label = "s == 0, f == dummy not null",
    .s = 0,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static create_test_params_t create_s_1_f_null = {
    .label = "s == 1, f == NULL",
    .s = 1,
    .f = NULL,
};

static create_test_params_t create_s_1_f_dummy_not_null = {
    .label = "s == 1, f == dummy not null",
    .s = 1,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static create_test_params_t create_s_2_f_null = {
    .label = "s == 2, f == NULL",
    .s = 2,
    .f = NULL,
};

static create_test_params_t create_s_2_f_dummy_not_null = {
    .label = "s == 2, f == dummy not null",
    .s = 2,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    fake_malloc_returned_value_for_hashtable = malloc(sizeof(hashtable));
    assert_non_null(fake_malloc_returned_value_for_hashtable);
    fake_malloc_returned_value_for_buckets = malloc(param_s(state) * sizeof(list));
    assert_non_null(fake_malloc_returned_value_for_buckets);
    return 0;
}

static int create_teardown(void **state) {
    set_allocators(NULL, NULL);
    free(fake_malloc_returned_value_for_hashtable);
    free(fake_malloc_returned_value_for_buckets);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: s == 0
// Expected: returns NULL
// param:
//	- create_s_0_f_null
//	- create_s_0_f_dummy_not_null
static void create_returns_null_when_s_0(void **state) {
    assert_null(hashtable_create(param_s(state), param_f(state)));
}

// Given: s == 0
// Expected: calls neither DATA_STRUCTURE_MALLOC nor DATA_STRUCTURE_FREE
// param:
//	- create_s_0_f_null
//	- create_s_0_f_dummy_not_null
static void create_calls_neither_malloc_nor_free_when_s_0(void **state) {
    hashtable_create(param_s(state), param_f(state));
}

// Given: s > 0
// Expected: calls DATA_STRUCTURE_MALLOC(sizeof(hashtable)) for hashtable
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_calls_malloc_for_hashtable_with_right_params_when_s_not_0(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid an other mock call
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for hashtable fail
// Expected: returns NULL
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_null_when_malloc_for_hashtable_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(hashtable_create(param_s(state), param_f(state)));
}

// Given: malloc call for hashtable success
// Expected: calls DATA_STRUCTURE_MALLOC(size * sizeof(list)) for buckets field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for buckets field fail
// Expected: frees the pointer return by malloc call for hashtable
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_frees_hashtable_when_malloc_for_buckets_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_hashtable);
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized buckets field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->buckets, fake_malloc_returned_value_for_buckets);
    size_t memory_area_size = param_s(state) * sizeof(list);
    unsigned char zeros[memory_area_size];
    memset(zeros, 0, memory_area_size);
    assert_memory_equal(ret->buckets, zeros, memory_area_size);
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized size field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_int_equal(ret->size, param_s(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized destroy_value_fn field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->destroy_value_fn, param_f(state));
}



//-----------------------------------------------------------------------------
// destroy TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    size_t s; // 1,2
    hashtable_destroy_value_fn_t f;
    int number_of_entries; // 0,1,2,3
    boolean there_is_a_collision; // at the first entry and only one collision
    boolean chars_are_dynamically_allocated;
    hashtable *ht;
    entry *entries; // values are chars
} destroy_test_params_t;

/*
description of non trivial cases that will tested:
note : entries put from the first bucket and collision only at first bucket

cases s == 1, number_of_entries == 0: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 0
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: dummy
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 1, number_of_entries == 1: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 1 (put at first place)
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 1, number_of_entries == 2: (4 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 2
    there_is_a_collision: TRUE or FALSE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 0: (2 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 0
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: dummy
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 1: (2 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 1
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 2: (4 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 2
    there_is_a_collision: TRUE or FALSE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 3: (4 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 3
    there_is_a_collision: TRUE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP
*/



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static size_t destroy_param_s(void **state) {
    return ((destroy_test_params_t *) *state)->s;
}

static hashtable_destroy_value_fn_t destroy_param_f(void **state) {
    return ((destroy_test_params_t *) *state)->f;
}

static int destroy_param_number_of_entries(void **state) {
    return ((destroy_test_params_t *) *state)->number_of_entries;
}

static boolean destroy_param_there_is_a_collision(void **state) {
    return ((destroy_test_params_t *) *state)->there_is_a_collision;
}

static boolean destroy_param_chars_are_dynamically_allocated(void **state) {
    return ((destroy_test_params_t *) *state)->chars_are_dynamically_allocated;
}

static hashtable * destroy_param_ht(void **state) {
    return ((destroy_test_params_t *) *state)->ht;
}

static entry * destroy_param_entries(void **state) {
    return ((destroy_test_params_t *) *state)->entries;
}

static void make_entries(void **state) {
    int n = destroy_param_number_of_entries(state);
    if (n == 0)
        return;
    entry *entries = destroy_param_entries(state);
    entries = malloc(n * sizeof(entry));
    assert_non_null(destroy_param_entries(state));
    for (int i = 0; i < n; i++) {
        entries[i].key = strdup(KEY_VALUES[i]);
        assert_non_null(entries[i].key);
        if (destroy_param_chars_are_dynamically_allocated(state)) {
            entries[i].value = malloc(sizeof(char));
            assert_non_null(entries[i].value);
            * (char *) (entries[i].value) = STATIC_CHARS[i];
        }
        else {
            entries[i].value = &STATIC_CHARS[i];
        }
    }
}

static void put_entries_in_hashtable(state) {
    hashtable *ht = destroy_param_ht(state);
    assert_non_null(ht);
    switch (destroy_param_number_of_entries(state)) {
    case 0:
        break;
    case 1:

        break;
    case 2:

        break;
    default:
        assert_equal(0,1);
    }
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static destroy_test_params_t destroy_ht_null = {
    .label = "ht == NULL",
    .s = DUMMY_SIZE_T_VALUE,
    .f = DUMMY_POINTER_VALUE,
    .number_of_entries = DUMMY_INT_VALUE,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = NULL,
    .entries = DUMMY_POINTER_VALUE,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    hashtable *ht = destroy_param_ht(state);
    if (ht == HASHTABLE_DEFINED_IN_SETUP) {
        ht = malloc(sizeof(hashtable));
        assert_non_null(ht);
        ht->size = param_s(state) * sizeof(list);
        ht->destroy_value_fn = param_f(state);
        ht->buckets = malloc(ht->size * sizeof(list));
        assert_non_null(ht->buckets);
        make_entries(state);
        //put_entries_in_hashtable(state)
    }

    return 0;
}

static int destroy_teardown(void **state) {
    set_allocators(NULL, NULL);

    hashtable *ht = destroy_param_ht(state);
    free(ht->buckets);
    free(ht);

    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: do nothing
// param:
//	- destroy_ht_null
static void destroy_returns_null_when_s_0(void **state) {
    hashtable_destroy(destroy_param_ht(state));
}





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest create_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, &create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, &create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, &create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, &create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, &create_s_2_f_dummy_not_null),
    };

    const struct CMUnitTest destroy_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            destroy_returns_null_when_s_0,
            create_setup, create_teardown, &destroy_ht_null),

    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_tests, NULL, NULL);
    failed += cmocka_run_group_tests(destroy_tests, NULL, NULL);

    return failed;
}
