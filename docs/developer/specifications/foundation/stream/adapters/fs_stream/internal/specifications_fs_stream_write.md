@page specifications_fs_stream_write fs_stream_write() specifications

# Signature

~~~c
static size_t fs_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

# Purpose

Implement the `stream` port write callback for the `fs_stream` adapter.

This callback writes up to `n` bytes from `buf` to the OSAL file handle owned by
the `fs_stream` backend.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the behavior exposed through `stream_write()` for stream
instances created by the `fs_stream` adapter.

See:
- @ref specifications_stream_write

# Preconditions

- `backend` must designate a valid `fs_stream_t`.
- `backend->state.file` must designate a valid open `OSAL_FILE`.
- `backend->file_ops` must designate a valid OSAL file operations table.
- `backend->file_ops->write` must not be `NULL`.
- `buf` must designate at least `n` readable bytes.
- `n` must be greater than `0`.

# Invalid arguments

- None.

# Success

- Delegates the write operation to `osal_file_ops_t::write`.
- Calls `osal_file_ops_t::write` with:
    - `ptr == buf`,
    - `size == 1`,
    - `nmemb == n`,
    - `stream == backend->state.file`.
- Returns the number of bytes written, as reported by
  `osal_file_ops_t::write`.
- If `st != NULL`, stores the mapped `stream_status_t` corresponding to the
  OSAL file status reported by the underlying write operation.

# Failure

- Returns the value reported by `osal_file_ops_t::write`.
- If `st != NULL`, stores the mapped `stream_status_t` corresponding to the
  OSAL file failure status.

# Ownership

- `backend` is borrowed.
- `buf` is borrowed.
- This function does not take ownership of either pointer.
- This function does not release the underlying OSAL file handle.

# Notes

- This callback is intended to be invoked through `stream_write()`.
- Public argument validation is performed by `stream_write()` before dispatch.
- Therefore, this callback treats the listed preconditions as internal
  invariants.
