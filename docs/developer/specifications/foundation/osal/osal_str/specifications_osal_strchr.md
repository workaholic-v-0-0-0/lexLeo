@page specifications_osal_strchr osal_strchr() specifications

# Signature

```c
char *osal_strchr(const char *s, int c);
```

# Purpose

Finds the first occurrence of a character in a null-terminated string.

# Preconditions

- `s` must point to a valid null-terminated string.
- `c` must be representable as an `unsigned char` or be equal to `'\0'`.

# Success

- Returns a pointer to the first occurrence of `c` in `s`.
- Returns a pointer to the terminating null byte if `c == '\0'`.
- Returns `NULL` if `c` is not found.

# Failure

- This operation does not report failure.

# Ownership

- No ownership is transferred.
- The caller retains ownership of `s`.
- The returned pointer, if non-`NULL`, points inside `s`.
