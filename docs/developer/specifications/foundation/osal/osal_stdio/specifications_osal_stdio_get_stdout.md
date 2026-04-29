@page specifications_osal_stdio_get_stdout osal_stdio_ops_t::get_stdout specifications

# Signature

```c
OSAL_STDIO *(*get_stdout)(void);
```

# Purpose

Returns the default OSAL handle for the process standard output stream.

# Success

- Returns a non-NULL `OSAL_STDIO *`.
- The returned handle represents the process standard output stream.
- The returned handle can be passed to compatible `osal_stdio_ops_t` stream operations.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the returned stream handle is not transferred to the caller.
- The caller must not destroy or modify the returned stream handle directly.
