@page specifications_osal_strcmp osal_strcmp() specifications

# Signature

```c
int osal_strcmp(const char *s1, const char *s2);
```

# Purpose

Compares two null-terminated strings lexicographically.

# Preconditions

- `s1` must point to a valid null-terminated string.
- `s2` must point to a valid null-terminated string.

# Success

- Returns `0` if `s1` and `s2` have the same contents.
- Returns a negative value if the first differing character in `s1`
  has a lower value than the corresponding character in `s2`.
- Returns a positive value if the first differing character in `s1`
  has a higher value than the corresponding character in `s2`.

# Failure

- This operation does not report failure.

# Ownership

- No ownership is transferred.
- The caller retains ownership of `s1` and `s2`.