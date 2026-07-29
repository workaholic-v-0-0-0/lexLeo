@page specifications_stream_flush stream_flush() specifications

# Signature

```c
stream_status_t stream_flush(stream_t *s);
```

# Purpose

Flush pending output associated with the stream `s`.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t`.

# Invalid arguments

- `s` must not be `NULL`.

# Success

For a valid argument:

- Delegates the flush operation to the backend bound to the stream.
- If the backend flush operation produces `STREAM_STATUS_OK`:
  - Returns `STREAM_STATUS_OK`.

# Failure

- For an invalid argument:
  - Returns `STREAM_STATUS_INVALID`.
- Otherwise, if the backend flush operation produces a status other than
  `STREAM_STATUS_OK`:
  - Returns the status produced by the backend flush operation.

