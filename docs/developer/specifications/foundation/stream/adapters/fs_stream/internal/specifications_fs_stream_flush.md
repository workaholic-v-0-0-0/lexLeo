@page specifications_fs_stream_flush fs_stream_flush() specifications

# Signature

~~~c
static stream_status_t fs_stream_flush(void *backend);
~~~

# Purpose

Implement the `stream` port flush callback for the `fs_stream` adapter.

This callback flushes the OSAL file handle owned by the `fs_stream_t`
designated by `backend`.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the behavior exposed through `stream_flush()` for stream
instances created by the `fs_stream` adapter.

See:

- @ref specifications_stream_flush

# Preconditions

- `backend` must designate a valid `fs_stream_t`.
- The `fs_stream_t` designated by `backend` must contain a valid OSAL file
  operations table and a valid open `OSAL_FILE`.

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

- The `fs_stream_t` designated by `backend` is borrowed.

# Notes

- This callback is intended to be invoked through `stream_flush()`.
