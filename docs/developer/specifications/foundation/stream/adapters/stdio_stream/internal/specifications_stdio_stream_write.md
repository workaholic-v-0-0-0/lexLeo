@page specifications_stdio_stream_write stdio_stream_write() specifications

# Signature

~~~c
static size_t stdio_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

# Purpose

Implement the `stream` port write callback for the `stdio_stream` adapter.

This callback writes up to `n` bytes from `buf` to the output-oriented standard
stream wrapped by the `stdio_stream_t` designated by `backend`.

# Relationship to the public port contract

This function is a private backend callback bound into the `stdio_stream`
vtable. It implements the behavior exposed through `stream_write()` for stream
instances created by the `stdio_stream` adapter.

See:

- @ref specifications_stream_write

# Preconditions

- If `backend != NULL`, `backend` must designate a valid `stdio_stream_t`.
- If `n > 0` and `buf != NULL`, `buf` must designate at least `n` readable
  bytes.
- The `stdio_stream_t` designated by `backend` must contain valid borrowed
  standard I/O operations and a valid standard stream.

# Invalid arguments

- If `backend == NULL`, returns `0` and, if `st != NULL`, stores
  `STREAM_STATUS_INVALID` in `*st`.
- If `n > 0` and `buf == NULL`, returns `0` and, if `st != NULL`, stores
  `STREAM_STATUS_INVALID` in `*st`.

# Success

- If `n == 0`, returns `0` without invoking the injected OSAL standard I/O
  write operation and, if `st != NULL`, stores `STREAM_STATUS_OK` in `*st`.
- Otherwise, delegates the write operation to the injected OSAL standard I/O
  write operation.
- Returns the number of bytes reported by the underlying OSAL standard I/O
  write operation.
- If `st != NULL`, stores `STREAM_STATUS_OK` in `*st`.

# Failure

- Returns `STREAM_STATUS_INVALID` through `st`, when `st != NULL`, for invalid
  arguments, and returns `0`.
- Returns `STREAM_STATUS_IO_ERROR` through `st`, when `st != NULL`, if the
  wrapped standard stream is `stdin`, and returns `0`.

# Ownership

- The `stdio_stream_t` designated by `backend` is borrowed.
- `buf` is borrowed.
- The wrapped standard stream is borrowed.
- No ownership is transferred.

# Notes

- This callback is intended to be invoked through `stream_write()`.
- Writing is supported only for output-oriented standard streams:
    - `stdout`
    - `stderr`
- Writing to `stdin` is rejected.
- The returned value is expressed in bytes.
