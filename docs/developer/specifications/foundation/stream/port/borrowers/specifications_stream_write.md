@page specifications_stream_write stream_write() specifications

# Signature

```c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
```

# Purpose

Write up to `n` bytes from `buf` to the stream `s`.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t`.

# Special cases

- If `n == 0`, the function returns `0`.
- If `n == 0` and `st != NULL`, the function sets `*st = STREAM_STATUS_OK`.
- In the `n == 0` case, no backend operation is performed.

# Invalid arguments

For `n > 0`:

- `s` must not be `NULL`.
- `buf` must not be `NULL`.

# Success

For `n > 0` and valid arguments:

- Delegates the write operation to the backend bound to the stream.
- If the backend write operation produces `STREAM_STATUS_OK`:
    - Returns the value produced by the backend write operation.
    - If `st != NULL`, sets `*st = STREAM_STATUS_OK`.

For `n == 0`:

- Returns `0`.
- If `st != NULL`, sets `*st = STREAM_STATUS_OK`.

# Failure

- For invalid arguments:
    - Returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
- Otherwise, if the backend write operation produces a status other than
  `STREAM_STATUS_OK`:
    - Returns the value produced by the backend write operation.
    - If `st != NULL`, stores in `*st` the status produced by the backend write
      operation.
