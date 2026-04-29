@page specifications_osal_stdio_default_ops osal_stdio_default_ops() specifications

# Signature

```c
const osal_stdio_ops_t *osal_stdio_default_ops(void);
```

# Purpose

Returns the default injectable OSAL standard I/O operations table.

# Success

- Returns a non-NULL pointer to an `osal_stdio_ops_t`.
- The returned table provides callable function pointers for:
    - `get_stdin`
    - `get_stdout`
    - `get_stderr`
    - `read`
    - `write`
    - `flush`
    - `error`
    - `eof`
    - `clear_error`
- The returned table is suitable for accessing the process standard streams through the default backend.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the returned operations table is not transferred to the caller.
- The caller must not destroy or modify the returned operations table.
