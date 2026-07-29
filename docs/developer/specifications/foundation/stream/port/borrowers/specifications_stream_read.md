@page specifications_stream_read stream_read() specifications

# Signature

```c
size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
```

# Purpose

Read up to `n` bytes from the stream `s` into `buf`.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t`.

# Invalid arguments

For `n > 0`:

- `s` must not be `NULL`.
- `buf` must not be `NULL`.

# Success

For `n > 0` and valid arguments:

- Delegates the read operation to the backend bound to the stream.
- If the backend read operation produces `STREAM_STATUS_OK`:
    - Returns the value produced by the backend read operation.
    - If `st != NULL`, sets `*st = STREAM_STATUS_OK`.
- If the backend read operation produces `STREAM_STATUS_EOF`:
    - Returns the value produced by the backend read operation.
    - If `st != NULL`, sets `*st = STREAM_STATUS_EOF`.

For `n == 0`:
    - Returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_OK`.
    - `buf` is unchanged.

# Failure

- For invalid arguments:
    - Returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
    - `buf` is unchanged.
- Otherwise, if the backend read operation produces a status other than
  `STREAM_STATUS_OK` or `STREAM_STATUS_EOF`:
    - Returns the value produced by the backend read operation.
    - If `st != NULL`, stores in `*st` the status produced by the backend read
      operation.
    - `buf` is unchanged.
