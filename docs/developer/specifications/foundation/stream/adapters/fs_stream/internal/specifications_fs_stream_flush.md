@page specifications_fs_stream_flush fs_stream_flush() specifications

# Signature

~~~c
static stream_status_t fs_stream_flush(void *backend);
~~~

# Purpose

Implement the `stream` port flush callback for the `fs_stream` adapter.

This callback flushes the OSAL file handle owned by the `fs_stream` backend.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the behavior exposed through `stream_flush()` for stream
instances created by the `fs_stream` adapter.

See:
- @ref specifications_stream_flush

# Preconditions

- `backend` must designate a valid `fs_stream_t`.
- `backend->state.file` must designate a valid open `OSAL_FILE`.
- `backend->file_ops` must designate a valid OSAL file operations table.
- `backend->file_ops->flush` must not be `NULL`.

# Invalid arguments

- None.

# Success

- Delegates the flush operation to the injected OSAL file flush operation.
- Returns the mapped `stream_status_t` corresponding to the
  `osal_file_status_t` reported by the underlying OSAL file flush operation.

# Failure

- None handled directly by this callback.
- Any failure status reported by the underlying OSAL file flush operation is
  mapped to `stream_status_t` and returned.

# Ownership

- `backend` is borrowed.
- This function does not take ownership of `backend`.
- This function does not release the underlying OSAL file handle.

# Notes

- This callback is intended to be invoked through `stream_flush()`.
- Public argument validation is performed by `stream_flush()` before dispatch.
- Therefore, this callback treats the listed preconditions as internal
  invariants.
