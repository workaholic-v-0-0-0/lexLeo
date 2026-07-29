@page specifications_stdio_stream_read stdio_stream_read() specifications

# Signature

~~~c
static size_t stdio_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

# Purpose

Implement the `stream` port read callback for the `stdio_stream` adapter.

This callback reads up to `n` bytes from the input-oriented standard stream
wrapped by the `stdio_stream_t` designated by `backend` into `buf`.

# Relationship to the public port contract

This function is a private backend callback bound into the `stdio_stream`
vtable. It implements the behavior exposed through `stream_read()` for stream
instances created by the `stdio_stream` adapter.

See:

- @ref specifications_stream_read

# Preconditions

- If `backend != NULL`, `backend` must designate a valid `stdio_stream_t`.
- If `n > 0` and `buf != NULL`, `buf` must designate at least `n` writable
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
  read operation and, if `st != NULL`, stores `STREAM_STATUS_OK` in `*st`.
- Otherwise, delegates the read operation to the injected OSAL standard I/O
  read operation.
- Returns the number of bytes reported by the underlying OSAL standard I/O
  read operation.
- After the delegated read operation:
    - if the underlying standard stream reports an error, stores
      `STREAM_STATUS_IO_ERROR` in `*st`, when `st != NULL`;
    - otherwise, if the underlying standard stream reports that end-of-input
      has been encountered, stores `STREAM_STATUS_EOF` in `*st`, when
      `st != NULL`;
    - otherwise, stores `STREAM_STATUS_OK` in `*st`, when `st != NULL`.

# Failure

- Returns `STREAM_STATUS_INVALID` through `st`, when `st != NULL`, for invalid
  arguments, and returns `0`.
- Returns `STREAM_STATUS_IO_ERROR` through `st`, when `st != NULL`, if the
  wrapped standard stream is not `stdin`, and returns `0`.
- After invoking the injected OSAL standard I/O read operation, if the
  underlying standard stream reports an error, returns
  `STREAM_STATUS_IO_ERROR` through `st`, when `st != NULL`.
- An underlying standard I/O error does not imply that no bytes were read:
  the function returns the number of bytes reported by the underlying read
  operation.
- If no error is reported and the underlying standard stream reports that
  end-of-input has been encountered, returns `STREAM_STATUS_EOF` through
  `st`, when `st != NULL`.
- End-of-input does not imply that zero bytes were read: the delegated read
  operation may return bytes obtained before encountering the end of input.

# Ownership

- The `stdio_stream_t` designated by `backend` is borrowed.
- `buf` is borrowed.
- The wrapped standard stream is borrowed.
- No ownership is transferred.

# Notes

- This callback is intended to be invoked through `stream_read()`.
- Reading is supported only for the input-oriented standard stream:
    - `stdin`
- Reading from any other wrapped standard stream is rejected.
- The returned value is expressed in bytes.