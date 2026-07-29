@page specifications_stdio_stream_close stdio_stream_close() specifications

# Signature

~~~c
static stream_status_t stdio_stream_close(
    void *backend);
~~~

# Purpose

Release the `stdio_stream_t` backend container associated with a borrowed
standard stream.

# Relationship to the public port contract

This function is a private backend callback bound into the `stdio_stream`
vtable. It implements the backend close step performed by `stream_destroy()`
for stream instances created by the `stdio_stream` adapter.

See:

- @ref specifications_stream_destroy

# Preconditions

- If `backend != NULL`, `backend` must designate a valid `stdio_stream_t`
  created by `stdio_stream_create()`.
- The `stdio_stream_t` designated by `backend` must contain valid borrowed
  memory operations, including a valid `free` operation.

# Invalid arguments

- If `backend == NULL`, returns `STREAM_STATUS_INVALID`.

# Success

- Releases the `stdio_stream_t` through its borrowed memory operations.
- Returns `STREAM_STATUS_OK`.

# Failure

- None other than invalid arguments handled directly by this callback.

# Ownership

- `backend` designates a `stdio_stream_t` owned by the caller and consumed by
  this operation on success.
- The wrapped standard stream is borrowed by the `stdio_stream_t`; its
  ownership is not transferred to this operation.
- The underlying standard stream is not closed.

# Notes

- This function is normally invoked through `stream_destroy()`.
- The backend container is released through `stdio_stream_t::mem_ops->free`.
- After a successful call, the caller must treat the backend pointer as
  invalid.
