@page specifications_stream_flush stream_flush() specifications

# Signature

```c
stream_status_t stream_flush(stream_t *s);
```

# Purpose

Flush pending output associated with the stream.

# Preconditions

- If `s != NULL`, `s` must denote a valid, possibly partially initialized
  `stream_t` handle created by `stream_create()`.

# Invalid arguments

- `s` must not be `NULL`.

# Success

When `s != NULL` and a backend is bound to the stream:

- Delegates the flush operation to the backend flush operation bound to the
  stream.
- Returns the status produced by the backend flush operation.

# Failure

- If `s == NULL`, returns `STREAM_STATUS_INVALID`.
- If `s != NULL` but no backend is bound to the stream, returns
  `STREAM_STATUS_NO_BACKEND`.

# Notes

- `stream_flush()` does not call the backend read, write, or close operations.
- When a backend is bound to the stream, it must be compatible with the stream
  vtable installed by `stream_create()`.
