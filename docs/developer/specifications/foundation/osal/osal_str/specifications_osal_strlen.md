@page specifications_osal_strlen osal_strlen() specifications

# Signature

```c
size_t osal_strlen(const char *s);
```

# Purpose

Returns the length of a null-terminated string.

# Preconditions

- `s` must point to a valid null-terminated string.

# Success

- Returns the number of characters before the terminating null byte.

# Failure

- This operation does not report failure.

# Ownership

- No ownership is transferred.
- The caller retains ownership of `s`.
