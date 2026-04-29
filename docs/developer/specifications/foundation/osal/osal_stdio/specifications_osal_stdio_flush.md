@page specifications_osal_stdio_flush osal_stdio_ops_t::flush specifications

# Signature

```c
int (*flush)(OSAL_STDIO *stdio);
```

# Purpose

Flushes pending output for an OSAL standard I/O stream.

# Success

- Returns `0` when the stream is successfully flushed.
- Makes pending buffered output visible according to the backend behavior.

# Failure

- Returns a non-zero value if the flush operation fails.
- Backend error state can be queried through the compatible `error` operation.

# Ownership

- Ownership of `stdio` is not transferred.
