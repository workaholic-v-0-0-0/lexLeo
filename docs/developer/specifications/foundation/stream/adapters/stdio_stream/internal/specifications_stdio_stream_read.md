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

Read up to `n` bytes from the input-oriented standard stream wrapped by the
`stdio_stream` backend into `buf`.

# Preconditions

- If `backend != NULL`, `backend` must point to a valid `stdio_stream_t`.
- If `n > 0`, `buf` must designate at least `n` writable bytes.

# Invalid arguments

- `backend` must not be `NULL`.
- If `n > 0`, `buf` must not be `NULL`.

# Success

- Returns the number of bytes read from the wrapped standard stream.
- If `st != NULL`, stores `STREAM_STATUS_OK` in `*st`.
- If `n == 0`, returns `0` and, if `st != NULL`, stores
  `STREAM_STATUS_OK` in `*st`.

# Failure

- Returns `0` and, if `st != NULL`, stores `STREAM_STATUS_INVALID`
  for invalid arguments.
- Returns `0` and, if `st != NULL`, stores `STREAM_STATUS_IO_ERROR`
  when the wrapped standard stream does not support reading.

# Notes

- This function is an internal backend callback used by the borrower-side
  `stream_read()` operation.
- The read is forwarded through the injected `osal_stdio_ops_t::read`
  callback bound in the backend handle.
- The returned value is expressed in bytes.
- This function does not take ownership of `backend` or `buf`.
- Reading is supported only for the input-oriented standard stream:
    - `stdin`
- Reading from output-oriented standard streams is rejected by contract:
    - `stdout`
    - `stderr`
