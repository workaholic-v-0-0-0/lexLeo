@page specifications_fs_stream_close fs_stream_close() specifications

# Signature

~~~c
static stream_status_t fs_stream_close(void *backend);
~~~

# Purpose

Implement the `stream` port close callback for the `fs_stream` adapter.

This callback closes the OSAL file handle owned by the `fs_stream` backend.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the backend close step performed by `stream_destroy()` for stream
instances created by the `fs_stream` adapter.

See:
- @ref specifications_stream_destroy

# Preconditions

- `backend` must designate a valid `fs_stream_t`.
- `backend->state.file` must designate a valid open `OSAL_FILE`.
- `backend->file_ops` must designate a valid OSAL file operations table.
- `backend->file_ops->close` must not be `NULL`.

# Invalid arguments

- None.

# Success

- Delegates the close operation to the injected OSAL file close operation.
- Returns the mapped `stream_status_t` corresponding to the
  `osal_file_status_t` reported by the underlying OSAL file close operation.
- Releases the adapter-owned backend resources.

# Failure

- None handled directly by this callback.
- Any failure status reported by the underlying OSAL file close operation is
  mapped to `stream_status_t` and returned.
- Adapter-owned backend resources are still released before returning.

# Ownership

- `backend` is borrowed.
- This function releases the adapter-owned backend resources before returning.
- The underlying OSAL file handle is released only through the injected OSAL
  file close operation.

# Notes

- This callback is intended to be invoked through `stream_destroy()`.
- Public argument validation is performed by `stream_destroy()` before dispatch.
- Therefore, this callback treats the listed preconditions as internal
  invariants.
- Backend cleanup is completed even if the underlying OSAL file close operation
  reports a failure. The mapped status is still returned to the caller.
