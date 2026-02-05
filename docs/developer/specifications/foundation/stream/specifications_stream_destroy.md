@page specifications_stream_destroy stream_destroy() specifications

# Signature

    void stream_destroy(stream_t **s);

# Purpose

Destroy a previously created public `stream_t` handle of the `stream` port.

# Success

- If `s == NULL`, the function does nothing.
- If `s != NULL` and `*s == NULL`, the function does nothing.
- Otherwise, the function releases the stream object referenced by `*s`.
- After destruction, `*s` is set to `NULL`.

# Failure

- None.

# Notes

- `stream_destroy()` is the matching destructor for handles created by
  `stream_create()`.
- This function provides a best-effort idempotent destruction pattern at the
  handle level because passing a `NULL` handle pointer or an already-`NULL`
  handle is accepted as a no-op.
