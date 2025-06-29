// src/data_structures/src/internal/hashtable_test_utils.c

#include "internal/hashtable_test_utils.h"

#ifdef UNIT_TEST

hash_djb2_fn hash_djb2_mockable = real_hash_djb2;
unsigned long real_hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}
void set_hash_djb2(hash_djb2_fn f) {
    hash_djb2_mockable = f ? f : real_hash_djb2;
}

#endif
