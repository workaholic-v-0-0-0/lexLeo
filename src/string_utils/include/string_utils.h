// src/string_utils/include/string_utils.h

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

typedef int(*string_compare_t)(const char *s1, const char *s2);
typedef char *(*string_duplicate_t)(const char *s);

void set_string_compare(string_compare_t string_compare);
string_compare_t get_current_string_compare(void);

void set_string_duplicate(string_duplicate_t string_duplicate);
string_duplicate_t get_current_string_duplicate(void);

#endif //STRING_UTILS_H


