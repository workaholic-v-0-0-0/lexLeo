@page specifications_osal_stdio_write osal_stdio_ops_t::write specifications

# Signature

```c
size_t (*write)(
    const void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_STDIO *stdio);
```

# Purpose

Writes data from a caller-provided buffer to an OSAL standard I/O stream.

# Success

- Returns the number of complete elements written.
- Writes at most `nmemb` elements of `size` bytes each.
- Reads the bytes to write from `ptr`.
- Advances the stream write position according to the backend behavior.

# Failure

- Returns a value smaller than `nmemb` if fewer complete elements can be written.
- Returns `0` if no complete element can be written.
- Backend error state can be queried through the compatible `error` operation.

# Ownership

- Ownership of `ptr` is not transferred.
- Ownership of `stdio` is not transferred.
