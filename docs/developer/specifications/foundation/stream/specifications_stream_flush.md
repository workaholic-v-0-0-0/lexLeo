@page specifications_stream_flush stream_flush() specifications

# Signature

    stream_status_t stream_flush(stream_t *s);

# Purpose

Flush pending output associated with the `stream` port.

# Preconditions

- If `s != NULL`, `s` must denote a valid `stream_t` handle created by
  `stream_create()`.

# Invalid arguments

- `s` must not be `NULL`.

# Success

When `s != NULL` and `s->backend != NULL`:

- Delegates the flush operation to the borrower-facing `flush` callback stored
  in the stream handle.
- Returns the status produced by the underlying `flush` callback.

# Failure

- If `s == NULL`, returns `STREAM_STATUS_INVALID`.
- If `s != NULL` but `s->backend == NULL`, returns
  `STREAM_STATUS_NO_BACKEND`.

# Notes

- `stream_flush()` does not call the borrower-facing `read`, `write`, or
  `close` callbacks.
- This function requires a stream handle whose adapter-facing virtual table has
  been validated at creation time by `stream_create()`.