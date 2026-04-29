@page specifications_osal_stdio_eof osal_stdio_ops_t::eof specifications

# Signature

```c
int (*eof)(OSAL_STDIO *stdio);
```

# Purpose

Queries the end-of-file state of an OSAL standard I/O stream.

# Success

- Returns `0` if the end-of-file indicator is not currently set for the stream.
- Returns a non-zero value if the end-of-file indicator is currently set for the stream.

# Failure

- This operation does not report failure separately from the queried end-of-file state.

# Ownership

- Ownership of `stdio` is not transferred.
