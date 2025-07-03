// bison/string_utils/bison/string_utils.c

#include "string_utils.h"

#include <string.h>

static string_compare_t current_string_compare = strcmp;
static string_duplicate_t current_string_duplicate = strdup;

void set_string_compare(string_compare_t string_compare) {
    current_string_compare = string_compare ? string_compare : strcmp;
}

void set_string_duplicate(string_duplicate_t string_duplicate) {
    current_string_duplicate = string_duplicate ? string_duplicate : strdup;
}

string_compare_t get_current_string_compare(void) {
    return current_string_compare;
}

string_duplicate_t get_current_string_duplicate(void) {
    return current_string_duplicate;
}