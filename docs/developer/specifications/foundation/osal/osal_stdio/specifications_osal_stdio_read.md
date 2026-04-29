@page specifications_osal_stdio_read osal_stdio_ops_t::read specifications

# Signature

```c
size_t (*read)(
    void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_STDIO *stdio);
```

# Purpose

Reads data from an OSAL standard I/O stream into a caller-provided buffer.

# Success

- Returns the number of complete elements read.
- Reads at most `nmemb` elements of `size` bytes each.
- Stores the read bytes into `ptr`.
- Advances the stream read position according to the backend behavior.

# Failure

- Returns a value smaller than `nmemb` if fewer complete elements can be read.
- Returns `0` if no complete element can be read.
- Backend error and end-of-file state can be queried through compatible
  `error` and `eof` operations.

# Ownership

- Ownership of `ptr` is not transferred.
- Ownership of `stdio` is not transferred.
