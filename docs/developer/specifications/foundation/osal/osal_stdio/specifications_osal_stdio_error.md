@page specifications_osal_stdio_error osal_stdio_ops_t::error specifications

# Signature

```c
int (*error)(OSAL_STDIO *stdio);
```

# Purpose

Queries the error state of an OSAL standard I/O stream.

# Success

- Returns `0` if no error indicator is currently set for the stream.
- Returns a non-zero value if an error indicator is currently set for the stream.

# Failure

- This operation does not report failure separately from the queried error state.

# Ownership

- Ownership of `stdio` is not transferred.
