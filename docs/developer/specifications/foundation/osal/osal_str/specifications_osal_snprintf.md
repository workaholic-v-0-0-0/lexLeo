@page specifications_osal_snprintf osal_snprintf() specifications

# Signature

```c
int osal_snprintf(char *str, size_t size, const char *format, ...);
```

# Purpose

Formats a string into a bounded character buffer using `printf`-style formatting.

# Preconditions

- If `str != NULL`, `str` must point to a writable character buffer of at least `size` bytes.
- If `format != NULL`, `format` must point to a valid null-terminated format string.
- Variadic arguments must match the conversion specifiers used by `format`.

# Invalid arguments

- `format == NULL`.
- `size > 0 && str == NULL`.

# Success

- Returns the number of characters that would have been written, excluding the terminating null byte.
- If the return value is non-negative and less than `size`, the formatted output was fully written.
- If the return value is greater than or equal to `size`, the output was truncated.
- If `size > 0`, the written output is null-terminated.

# Failure

- Returns `-1` for invalid arguments.
- If `format == NULL` and `str != NULL`, no byte in the buffer pointed to by `str` is modified.
- Returns a negative value if the underlying formatting operation fails.

# Ownership

- No ownership is transferred.
- The caller retains ownership of `str`.
