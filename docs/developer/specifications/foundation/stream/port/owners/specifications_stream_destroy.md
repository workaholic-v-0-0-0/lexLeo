@page specifications_stream_destroy stream_destroy() specifications

# Signature

~~~c
void stream_destroy(stream_t **s);
~~~

# Purpose

Destroy a previously created public `stream_t` handle of the `stream` port.

If a backend is bound, the function attempts backend cleanup through the
stream vtable `close` callback, then releases the public `stream_t` handle.

# Preconditions

If `s != NULL` and `*s != NULL`, `*s` must point to a valid `stream_t`.

# Success

- If `s == NULL`, the function does nothing.
- If `s != NULL` and `*s == NULL`, the function does nothing.
- Otherwise:
    - if a backend is bound, delegates backend cleanup to the stream vtable
      `close` callback;
    - releases the public `stream_t` handle;
    - stores `NULL` in `*s`.
 
# Failure

- None reported to the caller.
- Any failure status returned by the backend `close` callback is ignored.
- The public `stream_t` handle is still released and `*s` is still set to
  `NULL`.

# Ownership

- If a stream object is present, ownership of its bound backend and of the
  public `stream_t` handle is released by this function.

# Notes

- `stream_destroy()` is the matching destructor for handles created by
  `stream_create()`.
- A backend cleanup failure does not preserve the public handle.

