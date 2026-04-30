@page specifications_osal_str_ops_strdup osal_str_ops_t::strdup() specifications

# Signature

```c
char *(*strdup)(const char *s, const osal_mem_ops_t *mem_ops);
```

# Purpose

Duplicates a null-terminated string using an injected OSAL memory provider.

# Preconditions

- if `s != NULL`, `s` must point to a valid null-terminated string.
- if `mem_ops != NULL`, `mem_ops` must point to a valid `osal_mem_ops_t`.

# Invalid arguments

- `s` must not be `NULL`.
- `mem_ops` must not be `NULL`.

# Success

- Returns a non-NULL pointer to a newly allocated null-terminated copy of `s`.
- The returned string has the same contents as `s`.
- The returned pointer is distinct from `s`.
- The allocation size is `osal_strlen(s) + 1` bytes.
- The allocation is performed through `mem_ops->malloc`.

# Failure

- Returns `NULL` for invalid arguments.
- Returns `NULL` if `mem_ops->malloc` fails.

# Ownership

- On success, ownership of the returned string is transferred to the caller.
- The caller is responsible for releasing the returned string with the same memory provider.
- On failure, no string ownership is transferred.
- The return value does not distinguish invalid arguments from allocation failure.
