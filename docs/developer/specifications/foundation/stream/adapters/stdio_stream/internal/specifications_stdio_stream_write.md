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

Write up to `n` bytes from `buf` to the output-oriented standard stream wrapped
by the `stdio_stream` backend.

# Preconditions

- If `backend != NULL`, `backend` must point to a valid `stdio_stream_t`.
- If `n > 0`, `buf` must designate at least `n` readable bytes.

# Invalid arguments

- `backend` must not be `NULL`.
- If `n > 0`, `buf` must not be `NULL`.

# Success

- Returns the number of bytes written to the wrapped standard stream.
- If `st != NULL`, stores `STREAM_STATUS_OK` in `*st`.
- If `n == 0`, returns `0` and, if `st != NULL`, stores
  `STREAM_STATUS_OK` in `*st`.

# Failure

- Returns `0` and, if `st != NULL`, stores `STREAM_STATUS_INVALID`
  for invalid arguments.
- Returns `0` and, if `st != NULL`, stores `STREAM_STATUS_IO_ERROR`
  when the wrapped standard stream does not support writing.

# Notes

- This function is an internal backend callback used by the borrower-side
  `stream_write()` operation.
- The write is forwarded through the injected `osal_stdio_ops_t::write`
  callback bound in the backend handle.
- The returned value is expressed in bytes.
- This function does not take ownership of `backend` or `buf`.
- Writing is supported only for output-oriented standard streams:
  - `stdout`
  - `stderr`
- Writing to `stdin` is rejected by contract.
