@page specifications_stream_read stream_read() specifications

# Signature

~~~c
size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
~~~

# Purpose

Read up to `n` bytes from the `stream` port into `buf`.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t` handle created by
  `stream_create()` and correctly initialized via
  `stream_complete_default_init()`.

# Special cases

- If `n == 0`, the function returns `0`.
- If `n == 0` and `st != NULL`, the function sets `*st = STREAM_STATUS_OK`.
- In the `n == 0` case, no backend operation is performed.

# Invalid arguments

For `n > 0`:

- `s` must not be `NULL`.
- `buf` must not be `NULL`.

# Success

For `n > 0`, when `s != NULL`, `buf != NULL`, and a backend is bound to the
stream:

- Delegates the read operation to the backend read operation bound to the
  stream.
- Returns the value produced by the backend read operation.
- If `st != NULL`, stores in `*st` the status produced by the backend read
  operation.

# Failure

For `n > 0`:

- If `s == NULL`, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
- If `buf == NULL`, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
- If `s != NULL` but no backend is bound to the stream, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_NO_BACKEND`.

# Notes

- If `st == NULL`, status reporting is omitted.
- `stream_read()` does not call the backend write, flush, or close operations.
- This function requires a stream handle created by `stream_create()`.
- This function performs no backend operation when `n == 0`.
