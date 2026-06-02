@page specifications_stream_destroy stream_destroy() specifications

# Signature

~~~c
stream_status_t stream_destroy(stream_t **s);
~~~

# Purpose

Destroy a previously created public `stream_t` handle of the `stream` port.

This function attempts backend cleanup through the stream vtable `close`
callback when a backend is bound, then releases the public `stream_t` handle
regardless of the backend cleanup status.

# Success

- If `s == NULL`, returns `STREAM_STATUS_OK`.
- If `s != NULL` and `*s == NULL`, returns `STREAM_STATUS_OK`.
- Otherwise:
    - if a backend is bound, delegates backend cleanup to the stream vtable
      `close` callback;
    - releases the public `stream_t` handle;
    - stores `NULL` in `*s`;
    - returns the backend close status, or `STREAM_STATUS_OK` if no backend was
      bound.

# Failure

- If the backend `close` callback reports a failure status:
    - releases the public `stream_t` handle;
    - stores `NULL` in `*s`;
    - returns that status.

# Ownership

- `s` is borrowed.
- `*s` is consumed when `s != NULL` and `*s != NULL`.
- After destruction, `*s == NULL`, even if backend cleanup reports a failure.

# Notes

- `stream_destroy()` reports backend cleanup status.
- A backend close failure does not preserve the public handle.
- Passing `NULL` or a pointer to `NULL` is treated as a successful no-op.
