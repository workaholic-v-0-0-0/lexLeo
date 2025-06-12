// src/data_structures/tests/test_hashtable.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>

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
static hashtable * const HASHTABLE_DEFINED_IN_SETUP = (hashtable *) &dummy;
static entry * const ENTRIES_DEFINED_IN_SETUP = (entry *) &dummy;

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

static list collected_ptr_to_be_freed = NULL;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
    set_allocators(NULL, NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, ptr);
    set_allocators(mock_malloc, mock_free);
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

static void alloc_and_config_free(void **ptr, size_t size, boolean add_to_collected_ptr_to_be_freed) {
    *ptr = malloc(size);
    assert_non_null(*ptr);
    if (add_to_collected_ptr_to_be_freed) {
        set_allocators(NULL, NULL);
        collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, *ptr);
        set_allocators(mock_malloc, mock_free);
    }
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static const create_test_params_t create_s_0_f_null = {
    .label = "s == 0, f == NULL",
    .s = 0,
    .f = NULL,
};

static const create_test_params_t create_s_0_f_dummy_not_null = {
    .label = "s == 0, f == dummy not null",
    .s = 0,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static const create_test_params_t create_s_1_f_null = {
    .label = "s == 1, f == NULL",
    .s = 1,
    .f = NULL,
};

static const create_test_params_t create_s_1_f_dummy_not_null = {
    .label = "s == 1, f == dummy not null",
    .s = 1,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static const create_test_params_t create_s_2_f_null = {
    .label = "s == 2, f == NULL",
    .s = 2,
    .f = NULL,
};

static const create_test_params_t create_s_2_f_dummy_not_null = {
    .label = "s == 2, f == dummy not null",
    .s = 2,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_teardown(void **state) {
    set_allocators(NULL, NULL);
    list_free_list(collected_ptr_to_be_freed, free);
    collected_ptr_to_be_freed = NULL;
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
    fake_malloc_returned_value_for_hashtable = malloc(sizeof(hashtable));
    assert_non_null(fake_malloc_returned_value_for_hashtable);
    fake_malloc_returned_value_for_buckets = malloc(param_s(state) * sizeof(list));
    assert_non_null(fake_malloc_returned_value_for_buckets);
    set_allocators(NULL, NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, fake_malloc_returned_value_for_hashtable);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, fake_malloc_returned_value_for_buckets);
    set_allocators(mock_malloc, mock_free);
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
    alloc_and_config_free(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable), FALSE);
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
    alloc_and_config_free(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable), TRUE);
    alloc_and_config_free(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list), TRUE);
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
    alloc_and_config_free(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable), TRUE);
    alloc_and_config_free(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list), TRUE);
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
    alloc_and_config_free(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable), TRUE);
    alloc_and_config_free(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list), TRUE);
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

cases s == 1, number_of_entries == 2: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 2
    there_is_a_collision: TRUE
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

cases s == 2, number_of_entries == 3: (2 cases)
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

static void initialize_hashtable(destroy_test_params_t *params) {
	size_t s = params->s;
	hashtable *ht = NULL;
	if (s > 0) {
        alloc_and_config_free((void *)&ht, sizeof(hashtable), FALSE);
		//ht = malloc(sizeof(hashtable));
		assert_non_null(ht);
		ht->size = s;
		ht->destroy_value_fn = (params->ht)->destroy_value_fn;
		alloc_and_config_free((void *)&(ht->buckets), sizeof(list) * s, FALSE);
        //ht->buckets = malloc(sizeof(list) * s);
		assert_non_null(ht->buckets);
		for (size_t i = 0; i < s; ++i) {
			ht->buckets[i] = NULL;
		}
	}
	params->ht = ht;
}

static void initialize_entries(destroy_test_params_t *params) {
    int n = params->number_of_entries;
	entry *entries = NULL;
    if (n > 0) {
    	entries = malloc(n * sizeof(entry));
    	assert_non_null(entries);
    	for (int i = 0; i < n; i++) {
        	entries[i].key = strdup(KEY_VALUES[i]);
        	assert_non_null(entries[i].key);
        	if (params->chars_are_dynamically_allocated) {
            	entries[i].value = malloc(sizeof(char));
            	assert_non_null(entries[i].value);
            	* (char *) (entries[i].value) = STATIC_CHARS[i];
        	}
        	else {
            	entries[i].value = &STATIC_CHARS[i];
        	}
    	}
	}
	params->entries = entries;
}

static void destroy_entries(destroy_test_params_t *params) {
    int n = params->number_of_entries;
    if (n == 0)
        return;
	entry *entries = params->entries;
    for (int i = 0; i < n; i++) {
		free(entries[i].key);
        if (params->chars_are_dynamically_allocated) {
            free(entries[i].value);
        }
    }
    free(entries);
}

static void destroy_buckets(hashtable *ht) {
    list *buckets = ht->buckets;
    for (size_t i = 0; i < ht->size; i++) {
        while (buckets[i]) {
            list next = buckets[i]->cdr;
            free(buckets[i]);
            buckets[i] = next;
        }
    }
    free(buckets);
}

static void put_an_entry_in_hashtable(hashtable *ht, entry *entries, size_t entry_index, size_t bucket_index) {
    cons *c = malloc(sizeof(cons));
    assert_non_null(c);
    c->car = &entries[entry_index];
    c->cdr = ht->buckets[bucket_index];
    ht->buckets[bucket_index] = c;
}

static void put_entries_in_hashtable(destroy_test_params_t *params) {
    hashtable *ht = params->ht;
    assert_non_null(ht);
    int number_of_entries = params->number_of_entries;
    if (number_of_entries == 0)
        return;
    entry *entries = params->entries;
    if (number_of_entries >= 1) { // ht->buckets[0] = list_push(ht->buckets[0], car);
        put_an_entry_in_hashtable(ht, entries, 0, 0);
    }
    if (number_of_entries >= 2) {
        if (params->there_is_a_collision) {
            put_an_entry_in_hashtable(ht, entries, 1, 0);
        }
        else {
            put_an_entry_in_hashtable(ht, entries, 1, 1);
        }
    }
    if (number_of_entries >= 3) {
		assert(params->s == 2);
		assert(params->there_is_a_collision);
        put_an_entry_in_hashtable(ht, entries, 2, 1);
    }
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static const destroy_test_params_t destroy_params_ht_null = {
    .label = "ht == NULL",
    .s = DUMMY_SIZE_T_VALUE,
    .f = DUMMY_POINTER_VALUE,
    .number_of_entries = DUMMY_INT_VALUE,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = NULL,
    .entries = DUMMY_POINTER_VALUE,
};

static const destroy_test_params_t destroy_params_template_s_1_n_0_f_null = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_1_n_0_f_free = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_1_n_1_static = {
    .label = "hashtable size == 1, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_1_n_1_dynamic = {
    .label = "hashtable size == 1, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_1_n_2_collision_static = {
    .label = "hashtable size == 1, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_1_n_2_collision_dynamic = {
    .label = "hashtable size == 1, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_0_f_null = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_0_f_free = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_1_static = {
    .label = "hashtable size == 2, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_1_dynamic = {
    .label = "hashtable size == 2, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_2_no_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, no collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = FALSE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_2_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_2_no_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, no collision, dynamic allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = FALSE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_2_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, collision, dynamic allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_3_collision_static = {
    .label = "hashtable size == 2, number of entries == 3, collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 3,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};

static const destroy_test_params_t destroy_params_template_s_2_n_3_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 3,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_setup(void **state) {
	const destroy_test_params_t *model = *state;
    destroy_test_params_t *params = malloc(sizeof(destroy_test_params_t));
	assert_non_null(params);
	*params = *model;
	if (destroy_param_ht(state)) {
		initialize_hashtable(params);
		initialize_entries(params);
		put_entries_in_hashtable(params);
	}
    *state = params;
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_teardown(void **state) {
    set_allocators(NULL, NULL);
	destroy_test_params_t *params = *state;
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    free(params);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: does not call free
// param:
//	- destroy_params_ht_null
static void destroy_does_not_call_free_when_s_0(void **state) {
    hashtable_destroy(destroy_param_ht(state));
}

// Given: ht size == 1 or 2, no entry (empty buckets)
// Expected: frees ht->buckets and ht
// param:
//	- destroy_params_template_s_1_n_0_f_null
//	- destroy_params_template_s_1_n_0_f_free
//	- destroy_params_template_s_2_n_0_f_null
//	- destroy_params_template_s_2_n_0_f_free
static void destroy_frees_hashtable_when_no_entries(void **state) {
	expect_value(mock_free, ptr, destroy_param_ht(state)->buckets);
	expect_value(mock_free, ptr, destroy_param_ht(state));
    hashtable_destroy(destroy_param_ht(state));
}

// HERE




//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest create_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
    };

    const struct CMUnitTest destroy_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            destroy_does_not_call_free_when_s_0,
            destroy_setup, destroy_teardown, (void *)&destroy_params_ht_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, destroy_teardown, (void *)&destroy_params_template_s_1_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, destroy_teardown, (void *)&destroy_params_template_s_1_n_0_f_free),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, destroy_teardown, (void *)&destroy_params_template_s_2_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, destroy_teardown, (void *)&destroy_params_template_s_2_n_0_f_free),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_tests, NULL, NULL);
    failed += cmocka_run_group_tests(destroy_tests, NULL, NULL);

    return failed;
}
