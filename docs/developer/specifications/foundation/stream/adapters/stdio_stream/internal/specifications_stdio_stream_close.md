@page specifications_stdio_stream_close stdio_stream_close() specifications

# Signature

~~~c
static stream_status_t stdio_stream_close(
    void *backend);
~~~

# Purpose

Release the `stdio_stream` backend container associated with a borrowed
standard stream.

# Preconditions

- If `backend != NULL`, `backend` must point to a valid `stdio_stream_t`.

# Invalid arguments

- `backend` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Releases the backend container through the injected memory operations.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.

# Notes

- This function is the adapter-facing backend close callback bound in the
  `stream` virtual table.
- This function consumes the backend container on success.
- The backend container is released through
  `stdio_stream_t::mem_ops->free`.
- The wrapped standard stream is borrowed by the backend and is not owned by
  it.
- Therefore, this function releases the backend container but does not close
  the underlying standard stream itself.
- After a successful call, the caller must treat the backend pointer as
  invalid.