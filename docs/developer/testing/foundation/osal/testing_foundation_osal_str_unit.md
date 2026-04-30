@page testing_foundation_osal_str_unit osal_str unit tests

It covers:
- `osal_str_default_ops()`
- `osal_str_ops_t::strdup()`
- `osal_snprintf()`
- `osal_strlen()`
- `osal_strcmp()`
- `osal_strchr()`
- `osal_strrchr()`
- `osal_isspace()`

---

@anchor testing_foundation_osal_str_unit_default_ops
# osal_str_default_ops() unit tests

See @ref specifications_osal_str_default_ops "osal_str_default_ops() specifications"

## Functions under test

~~~c
const osal_str_ops_t *osal_str_default_ops(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_str_default_ops()` is called | returns a non-`NULL` pointer to an `osal_str_ops_t`;<br>`ret->strdup != NULL` |

---

@anchor testing_foundation_osal_str_unit_strdup
# osal_str_ops_t::strdup() unit tests

See @ref specifications_osal_str_ops_strdup "osal_str_ops_t::strdup() specifications"

## Functions under test

~~~c
char *(*strdup)(const char *s, const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `strdup()` is called with `s == NULL` | returns `NULL`;<br>no allocation is performed |
| `strdup()` is called with `mem_ops == NULL` | returns `NULL`;<br>no allocation is performed |
| `strdup()` is called with a valid empty string | returns a non-`NULL` pointer;<br>returned string is empty;<br>returned pointer is distinct from `s`;<br>allocation size is 1 byte |
| `strdup()` is called with a valid non-empty string | returns a non-`NULL` pointer;<br>returned string has the same contents as `s`;<br>returned pointer is distinct from `s`;<br>allocation size is `strlen(s) + 1` |
| `mem_ops->malloc` fails | returns `NULL` |

---

@anchor testing_foundation_osal_str_unit_osal_snprintf
# osal_snprintf() unit tests

See @ref specifications_osal_snprintf "osal_snprintf() specifications"

## Functions under test

~~~c
int osal_snprintf(char *str, size_t size, const char *format, ...);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_snprintf()` is called with `format == "abc"`, `str == NULL`, `size == 3`, and no optional argument | returns `-1` |
| `osal_snprintf()` is called with `format == NULL`, `str != NULL`, `size == 3`, and no optional argument | returns `-1`;<br>no byte in the buffer pointed to by `str` is modified |
| `osal_snprintf()` is called with `format == "number is %d"`, `str != NULL`, `size == 0`, and first optional argument `12345` | returns `15`;<br>no byte in the buffer pointed to by `str` is modified |
| `osal_snprintf()` is called with `format == "number is %d"`, `str == NULL`, `size == 0`, and first optional argument `12345` | returns `15` |
| `osal_snprintf()` is called with `format == "number is %d"`, `str != NULL`, `size == 15`, and first optional argument `12345` | returns `15`;<br>writes `"number is 1234"` to `str`;<br>`str` is null-terminated |
| `osal_snprintf()` is called with `format == "number is %d"`, `str != NULL`, `size == 16`, and first optional argument `12345` | returns `15`;<br>writes `"number is 12345"` to `str`;<br>`str` is null-terminated |
| `osal_snprintf()` is called with `format == "number is %d"`, `str != NULL`, `size == 32`, and first optional argument `12345` | returns `15`;<br>writes `"number is 12345"` to `str`;<br>`str` is null-terminated |

---

@anchor testing_foundation_osal_str_unit_osal_strlen
# osal_strlen() unit tests

See @ref specifications_osal_strlen "osal_strlen() specifications"

## Functions under test

~~~c
size_t osal_strlen(const char *s);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_strlen()` is called with a valid null-terminated string | returns the number of characters before the terminating null byte |

---

@anchor testing_foundation_osal_str_unit_osal_strcmp
# osal_strcmp() unit tests

See @ref specifications_osal_strcmp "osal_strcmp() specifications"

## Functions under test

~~~c
int osal_strcmp(const char *s1, const char *s2);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_strcmp()` is called with `s1 == "001"` and `s2 == "002"` | returns a negative value |

---

@anchor testing_foundation_osal_str_unit_osal_strchr
# osal_strchr() unit tests

See @ref specifications_osal_strchr "osal_strchr() specifications"

## Functions under test

~~~c
char *osal_strchr(const char *s, int c);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_strchr()` is called with `s == "this_will_be_cut_a1a2a3"` and `c == 'a'` | returns a non-`NULL` pointer;<br>returned string view is `"a1a2a3"` |

---

@anchor testing_foundation_osal_str_unit_osal_strrchr
# osal_strrchr() unit tests

See @ref specifications_osal_strrchr "osal_strrchr() specifications"

## Functions under test

~~~c
char *osal_strrchr(const char *s, int c);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_strrchr()` is called with `s == "this_will_be_cut_a1a2a3"` and `c == 'a'` | returns a non-`NULL` pointer;<br>returned string view is `"a3"` |

## Notes

- These tests validate the public default last-character search helper.
- `NULL` input is not tested because `s == NULL` violates the function preconditions.
- The returned pointer, when non-`NULL`, points inside the original string.

---

@anchor testing_foundation_osal_str_unit_osal_isspace
# osal_isspace() unit tests

See @ref specifications_osal_isspace "osal_isspace() specifications"

## Functions under test

~~~c
int osal_isspace(int c);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_isspace()` is called with `c == ' '` | returns a non-zero value |

## Notes

- These tests validate the public default whitespace classification helper.
- Inputs outside the valid `isspace` domain are not tested because they violate the function preconditions.
