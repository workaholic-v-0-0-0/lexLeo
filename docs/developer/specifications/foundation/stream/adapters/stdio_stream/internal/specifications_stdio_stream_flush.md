@page specifications_stdio_stream_flush stdio_stream_flush() specifications

# Signature

~~~c
static stream_status_t stdio_stream_flush(
    void *backend);
~~~

# Purpose

Flush the output-oriented standard stream wrapped by the `stdio_stream`
backend.

# Preconditions

- If `backend != NULL`, `backend` must point to a valid `stdio_stream_t`.

# Invalid arguments

- `backend` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_IO_ERROR` when the wrapped standard stream does not
  support flushing.

# Notes

- This function is the adapter-facing backend flush callback bound in the
  `stream` virtual table.
- The flush is forwarded through the injected `osal_stdio_ops_t::flush`
  callback bound in the backend handle.
- This function does not take ownership of `backend`.
- Flushing is supported only for output-oriented standard streams:
  - `stdout`
  - `stderr`
- Flushing `stdin` is rejected by contract.
