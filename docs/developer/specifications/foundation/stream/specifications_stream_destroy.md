@page specifications_stream_destroy stream_destroy() specifications

# Signature

~~~c
stream_status_t stream_destroy(stream_t **s);
~~~

# Purpose

Destroy a previously created public `stream_t` handle of the `stream` port.

This function performs backend cleanup through the stream vtable `close`
callback. If backend cleanup succeeds, it releases the public `stream_t` handle.
If backend cleanup fails, it reports the failure and leaves the handle unchanged.

# Success

- If `s == NULL`, returns `STREAM_STATUS_OK`.
- If `s != NULL` and `*s == NULL`, returns `STREAM_STATUS_OK`.
- Otherwise:
    - delegates backend cleanup to the stream vtable `close` callback;
    - if backend cleanup succeeds:
        - releases the public `stream_t` handle;
        - stores `NULL` in `*s`;
        - returns `STREAM_STATUS_OK`.

# Failure

- If the backend `close` callback reports a failure status:
    - returns that status;
    - leaves `*s` unchanged;
    - does not release the public `stream_t` handle.

# Ownership

- `s` is borrowed.
- `*s` is consumed only when `s != NULL`, `*s != NULL`, and backend cleanup
  succeeds.
- If backend cleanup succeeds, the public `stream_t` handle is released and
  `*s == NULL`.
- If backend cleanup fails, ownership of `*s` remains with the caller.

# Notes

- `stream_destroy()` reports backend cleanup status.
- If backend close fails, the caller can observe the returned status and may
  retry destruction because `*s` is left unchanged.
- Passing `NULL` or a pointer to `NULL` is treated as a successful no-op.
