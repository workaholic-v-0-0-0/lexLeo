@page specifications_stdio_stream_flush stdio_stream_flush() specifications

# Signature

~~~c
static stream_status_t stdio_stream_flush(
    void *backend);
~~~

# Purpose

Implement the `stream` port flush callback for the `stdio_stream` adapter.

This callback flushes the output-oriented standard stream wrapped by the
`stdio_stream_t` designated by `backend`.

# Relationship to the public port contract

This function is a private backend callback bound into the `stdio_stream`
vtable. It implements the behavior exposed through `stream_flush()` for stream
instances created by the `stdio_stream` adapter.

See:

- @ref specifications_stream_flush

# Preconditions

- If `backend != NULL`, `backend` must designate a valid `stdio_stream_t`.
- The `stdio_stream_t` designated by `backend` must contain valid borrowed
  standard I/O operations and a valid standard stream.

# Invalid arguments

- If `backend == NULL`, returns `STREAM_STATUS_INVALID`.

# Success

- If the wrapped standard stream is output-oriented, delegates the flush
  operation to the injected OSAL standard I/O flush operation.
- Returns `STREAM_STATUS_OK`.

# Failure

- Returns `STREAM_STATUS_INVALID` if `backend == NULL`.
- Returns `STREAM_STATUS_IO_ERROR` if the wrapped standard stream is `stdin`.

# Ownership

- The `stdio_stream_t` designated by `backend` is borrowed.
- The wrapped standard stream is borrowed.
- No ownership is transferred.

# Notes

- This callback is intended to be invoked through `stream_flush()`.
- Flushing is supported only for output-oriented standard streams:
    - `stdout`
    - `stderr`
- Flushing `stdin` is rejected.