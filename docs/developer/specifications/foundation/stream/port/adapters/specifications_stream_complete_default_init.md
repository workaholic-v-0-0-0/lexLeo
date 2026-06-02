@page specifications_stream_complete_default_init stream_complete_default_init() specifications

# Signature

```c
stream_status_t stream_complete_default_init(
    stream_t *stream,
    void *backend);
```

# Purpose

Complete the normal initialization lifecycle of a `stream_t` handle by
ensuring that its owned backend is initialized.

# Preconditions

- `stream` must point to a valid, partially initialized `stream_t` handle
  created by `stream_create()`.
- If `stream->backend == NULL`, `backend` must point to a valid backend
  instance compatible with `stream->vtbl`.
- If `stream->backend != NULL`, `backend` must be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- If `stream->backend == NULL`, transfers ownership of `backend` to the
  stream handle.
- Otherwise leaves the stream handle unchanged.
- On return, the stream handle is fully initialized.

# Failure

- None.

# Ownership

- When `backend` is not `NULL`, ownership of the backend is transferred to the
  stream handle.
- When `backend` is `NULL`, no ownership is transferred.
- The attached backend is later released by `stream_destroy()` through
  `stream_t::vtbl::close()`.

# Notes

- Passing `backend == NULL` explicitly indicates that the backend has already
  been injected (for example by white-box test helpers).
- This function completes the standard initialization path of a `stream_t`
  handle.
