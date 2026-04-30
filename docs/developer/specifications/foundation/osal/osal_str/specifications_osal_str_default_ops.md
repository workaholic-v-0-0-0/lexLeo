@page specifications_osal_str_default_ops osal_str_default_ops() specifications

# Signature

```c
const osal_str_ops_t *osal_str_default_ops(void);
```

# Purpose

Returns the default injectable OSAL string operations table.

# Success

- Returns a non-NULL pointer to an `osal_str_ops_t`.
- The returned table provides callable function pointers for:
    - `strdup`
- The `strdup` operation is intentionally not a direct alias of the standard `strdup`.
- Its signature requires an injected `osal_mem_ops_t` memory provider:
    ```c
    char *(*strdup)(const char *s, const osal_mem_ops_t *mem_ops);
    ```
- This allows the duplicated string to be allocated with the same memory provider that the caller is expected to use when releasing it.
- The returned table is suitable for string-related operations that require dependency injection and allocator consistency.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the returned operations table is not transferred to the caller.
- The caller must not destroy or modify the returned operations table.
