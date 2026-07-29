@page specifications_fs_stream_close fs_stream_close() specifications

# Signature

~~~c
static stream_status_t fs_stream_close(void *backend);
~~~

# Purpose

This callback closes, if present, the OSAL file handle owned by the
`fs_stream_t` backend and releases the `fs_stream_t`.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the backend close step performed by `stream_destroy()` for stream
instances created by the `fs_stream` adapter.

See:
- @ref specifications_stream_destroy

# Preconditions

- `backend` must designate an `fs_stream_t` created by `fs_stream_create()`.
- `backend->file_ops` must designate a valid file operations table.
- `backend->mem_ops` must designate valid memory operations.
- If `backend->file != NULL`, it must designate a valid `OSAL_FILE`.

# Success

- If `fs_stream->file == NULL`:
    - Releases the `fs_stream_t`.
    - Returns `STREAM_STATUS_OK`.
- Otherwise:
    - Delegates the close operation to the injected OSAL file close operation.
    - The underlying OSAL file close operation returns `OSAL_FILE_STATUS_OK`.
    - Sets `fs_stream->file` to `NULL`.
    - Releases the `fs_stream_t`.
    - Returns `STREAM_STATUS_OK`.

# Failure

- If `fs_stream->file == NULL`, no failure occurs and the function returns
  `STREAM_STATUS_OK`.
- Otherwise, the close operation is delegated to the injected OSAL file close
  operation.
- If the underlying OSAL file close operation returns a status other than
  `OSAL_FILE_STATUS_OK`:
    - The returned status is mapped to the corresponding `stream_status_t`.
    - `fs_stream->file` is set to `NULL`.
    - The `fs_stream_t` is released.
    - The mapped status is returned.

# Ownership

- `backend` designates an `fs_stream_t` owned by the caller and consumed by
  this operation.
- The `OSAL_FILE` owned by the `fs_stream_t` is passed to the injected OSAL
  file close operation.
- The `fs_stream_t` is released before returning.

# Notes

- It is normally invoked through `stream_destroy()`, and may also be used for
  cleanup during backend construction failure paths.
- The `fs_stream_t` is released even if the underlying OSAL file close operation
  reports a failure. The mapped status is still returned to the caller.
