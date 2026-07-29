@page specifications_stream_complete_default_init stream_complete_default_init() specifications

# Signature

```c
stream_status_t stream_complete_default_init(
    stream_t *stream,
    void *backend);
```

# Purpose

Complete the initialization of a partially initialized `stream_t` handle by
attaching its backend as an owned resource.

# Preconditions

- `stream` must point to a valid, partially initialized `stream_t` handle
  created by `stream_create()`.
- The external borrowed dependencies of `stream` must be correctly initialized.
- If `stream->backend == NULL`, `backend` must point to a valid backend
  instance compatible with `stream->vtbl`.
- If `stream->backend != NULL`, `backend` must be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- If `backend != NULL`, attaches `backend` to the stream handle as an owned
  resource.
- Otherwise, leaves the stream handle unchanged.
- Leaves all external borrowed dependencies of `stream` unchanged.
- The resulting `stream_t` is fully initialized and can be used through the
  `stream` port operations.

# Failure

- None.

# Ownership

- Ownership of `stream` is not transferred.
- The external borrowed dependencies stored in `stream` remain borrowed.
- When `backend != NULL`, ownership of the backend is transferred to the
  stream handle.
- When `backend == NULL`, no ownership is transferred.
- The owned backend is later released by `stream_destroy()` through
  `stream_t::vtbl::close()`.

# Notes

- This function completes the initialization started by `stream_create()`.
- `stream_create()` initializes the handle's external borrowed dependencies.
- `stream_complete_default_init()` initializes the remaining handle state,
  namely its owned backend resource.
- Passing `backend == NULL` explicitly indicates that the backend has already
  been injected, for example by white-box test helpers.
