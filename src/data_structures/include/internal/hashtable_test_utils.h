// src/data_structures/include/internal/hashtable_test_utils.h

#ifndef HASHTABLES_TEST_UTILS_H
#define HASHTABLES_TEST_UTILS_H

#include "internal/hashtable_internal.h"

#include "internal/data_structure_memory_allocator.h"
#include "internal/data_structure_string_utils.h"

typedef unsigned long (*hash_djb2_fn)(const char *str);
unsigned long real_hash_djb2(const char *str);
extern hash_djb2_fn hash_djb2_mockable;
void set_hash_djb2(hash_djb2_fn f);

#endif //HASHTABLES_TEST_UTILS_H
