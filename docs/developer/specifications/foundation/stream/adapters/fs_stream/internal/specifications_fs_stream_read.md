@page specifications_fs_stream_read fs_stream_read() specifications

# Signature

~~~c
static size_t fs_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

# Purpose

Implement the `stream` port read callback for the `fs_stream` adapter.

This callback reads up to `n` bytes from the OSAL file handle owned by the
`fs_stream_t` designated by `backend` into `buf`.

# Relationship to the public port contract

This function is a private backend callback bound into the `fs_stream` vtable.
It implements the behavior exposed through `stream_read()` for stream instances
created by the `fs_stream` adapter.

See:

- @ref specifications_stream_read

# Preconditions

- `backend` must designate a valid `fs_stream_t`.
- `buf` must designate at least `n` writable bytes.
- `n` must be greater than `0`.
- The `fs_stream_t` designated by `backend` must contain a valid OSAL file
  operations table and must own a valid open `OSAL_FILE`.

# Invalid arguments

- None.

# Success

- Delegates the read operation to the injected OSAL file read operation.
- Returns the number of bytes reported by the underlying OSAL file read
  operation.
- If `st != NULL`, stores the mapped `stream_status_t` corresponding to the
  `osal_file_status_t` reported by the underlying OSAL file read operation.

# Failure

- None handled directly by this callback.
- Any failure status reported by the underlying OSAL file read operation is
  mapped to `stream_status_t` and exposed through `st` when `st != NULL`.

# Ownership

- The `fs_stream_t` designated by `backend` is borrowed.
- `buf` is borrowed.

# Notes

- This callback is intended to be invoked through `stream_read()`.
- Public argument validation is performed by `stream_read()` before dispatch.
- Therefore, this callback treats the listed preconditions as internal
  invariants.
- The returned value is expressed in bytes.
