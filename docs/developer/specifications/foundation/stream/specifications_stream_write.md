@page specifications_stream_write stream_write() specifications

# Signature

    size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);

# Purpose

Write up to `n` bytes from `buf` to the `stream` port.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t` handle created by
  `stream_create()`.

# Special cases

- If `n == 0`, the function returns `0`.
- If `n == 0` and `st != NULL`, the function sets `*st = STREAM_STATUS_OK`.
- In the `n == 0` case, no backend operation is performed.

# Invalid arguments

For `n > 0`:

- `s` must not be `NULL`.
- `buf` must not be `NULL`.

# Success

For `n > 0`, when `s != NULL`, `buf != NULL`, and `s->backend != NULL`:

- Delegates the write operation to the borrower-facing `write` callback stored
  in the stream handle.
- Returns the value produced by the underlying `write` callback.
- If `st != NULL`, stores in `*st` the status produced by the underlying
  `write` callback.

# Failure

For `n > 0`:

- If `s == NULL`, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
- If `buf == NULL`, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_INVALID`.
- If `s != NULL` but `s->backend == NULL`, returns `0`.
    - If `st != NULL`, sets `*st = STREAM_STATUS_NO_BACKEND`.

# Notes

- If `st == NULL`, status reporting is omitted.
- `stream_write()` does not call the borrower-facing `read`, `flush`, or
  `close` callbacks.
- This function performs no backend operation when `n == 0`.
- This function requires a stream handle whose borrower-facing virtual table has
  been validated at creation time by `stream_create()`.
