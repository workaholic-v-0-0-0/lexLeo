@page testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled stream / stdio_stream integration tests with doubled OSAL stdio dependency

---

@anchor testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_smoke

# `stream` / `stdio_stream` smoke tests

See:

- @ref specifications_stdio_stream_create_adapter_provider "stdio_stream_create_adapter_provider() specifications"
- @ref specifications_stream_create_factory "stream_create_factory() specifications"
- @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications"
- @ref specifications_stream_create_standard_stream_creator "stream_create_standard_stream_creator() specifications"
- @ref specifications_stream_standard_stream_creator_create "stream_standard_stream_creator_create() specifications"
- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Test doubles

- fake OSAL stdio dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| a `stdio_stream` adapter provider is registered in a stream factory and a standard-input stream is created through it | creation succeeds, the fake `stdin` getter is invoked, the created stream reads the expected buffered standard-input content through `stdio_stream`, and destroying the stream releases the stream backend |

---

@anchor testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_read

# `stream_read()` / `stdio_stream` integration tests

See:

- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stdio_stream_read "stdio_stream_read() specifications"

## Function under test

~~~c
size_t stream_read(
    stream_t *s,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Test doubles

- fake OSAL stdio dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `n > 0` and `s == NULL` | `stream_read()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the destination buffer unchanged |
| `n > 0` and `buf == NULL` | `stream_read()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the destination buffer unchanged |
| the wrapped standard stream is `stdout` | `stream_read()` returns `0`, reports `STREAM_STATUS_IO_ERROR`, and leaves the destination buffer unchanged |
| the underlying standard input reports an I/O error after reading one byte | `stream_read()` returns the byte count reported by the underlying read operation, preserves the byte copied to the destination buffer, and reports `STREAM_STATUS_IO_ERROR` |
| `n == 0` and `s == NULL` | `stream_read()` returns `0`, sets `*st = STREAM_STATUS_OK`, and leaves the destination buffer unchanged |
| `n` exceeds the number of currently buffered bytes and end-of-input has not been signaled | `stream_read()` returns the number of available bytes, copies those bytes to the destination buffer, and reports `STREAM_STATUS_OK` |
| `n` exactly matches the number of currently buffered bytes | `stream_read()` returns `n`, copies the requested bytes to the destination buffer, and reports `STREAM_STATUS_OK` |
| the read position is already at the end of the buffered input and end-of-input has not been signaled | `stream_read()` returns `0`, reports `STREAM_STATUS_OK`, and leaves the destination buffer unchanged |
| fewer than the currently buffered bytes are requested | `stream_read()` returns `n`, copies the requested bytes to the destination buffer, and reports `STREAM_STATUS_OK` |
| two reads are performed while buffered input remains after the second read | both reads return the requested byte count and report `STREAM_STATUS_OK`; the second read continues from the position reached by the first |
| end-of-input has been signaled and fewer than the remaining buffered bytes are requested | `stream_read()` returns `n`, copies the requested bytes, and reports `STREAM_STATUS_OK` because end-of-input has not yet been encountered |
| end-of-input has been signaled and `n` exactly matches the remaining buffered bytes | `stream_read()` returns `n`, copies all remaining buffered bytes, and reports `STREAM_STATUS_OK` because the read request is fully satisfied without encountering end-of-input |
| end-of-input has been signaled and `n` exceeds the number of remaining buffered bytes | `stream_read()` returns the number of remaining buffered bytes, copies those bytes to the destination buffer, and reports `STREAM_STATUS_EOF` |
| end-of-input has been signaled, the first read consumes exactly all remaining buffered data, and a second read is attempted | the first read returns the requested byte count and reports `STREAM_STATUS_OK`; the second returns `0` and reports `STREAM_STATUS_EOF` |

---

@anchor testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_write

# `stream_write()` / `stdio_stream` integration tests

See:

- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stdio_stream_write "stdio_stream_write() specifications"

## Function under test

~~~c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Test doubles

- fake OSAL stdio dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `n > 0` and `s == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the fake stdio buffered content unchanged |
| `n > 0` and `buf == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the fake stdio buffered content unchanged |
| the wrapped standard stream is `stdin` | `stream_write()` returns `0`, reports `STREAM_STATUS_IO_ERROR`, and leaves the fake stdio buffered content unchanged |
| the underlying standard output reports an I/O error after writing one byte | `stream_write()` returns the byte count reported by the underlying write operation, preserves the byte written to the fake buffered content, and reports `STREAM_STATUS_IO_ERROR` |
| `n == 0` and `s == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_OK`, and leaves the fake stdio buffered content unchanged |
| a nonzero write is performed on `stdout` | `stream_write()` returns the requested byte count, reports `STREAM_STATUS_OK`, and appends the supplied bytes to the fake stdio buffered content |
| a nonzero write is performed on `stderr` | `stream_write()` returns the requested byte count, reports `STREAM_STATUS_OK`, and appends the supplied bytes to the fake stdio buffered content |
| two consecutive writes are performed on `stdout` | both writes return their requested byte counts and report `STREAM_STATUS_OK`; the second write appends its bytes after the content produced by the first |

---

@anchor testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_flush

# `stream_flush()` / `stdio_stream` integration tests

See:

- @ref specifications_stream_flush "stream_flush() specifications"
- @ref specifications_stdio_stream_flush "stdio_stream_flush() specifications"

## Function under test

~~~c
stream_status_t stream_flush(stream_t *s);
~~~

## Test doubles

- fake OSAL stdio dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `s == NULL` | `stream_flush()` returns `STREAM_STATUS_INVALID` and leaves the fake stdio buffered and sink contents unchanged |
| the wrapped standard stream is `stdin` | `stream_flush()` returns `STREAM_STATUS_IO_ERROR` and leaves the fake stdio buffered and sink contents unchanged |
| the OSAL stdio flush operation reports failure after transferring the buffered output | `stream_flush()` returns `STREAM_STATUS_IO_ERROR`, the buffered content is emptied, and the transferred bytes remain appended to the sink content |
| the wrapped standard stream is `stdout` and the OSAL stdio flush operation succeeds | `stream_flush()` returns `STREAM_STATUS_OK`, empties the buffered content, and appends the buffered bytes to the existing sink content |
| the wrapped standard stream is `stderr` and the OSAL stdio flush operation succeeds | `stream_flush()` returns `STREAM_STATUS_OK`, empties the buffered content, and appends the buffered bytes to the existing sink content |
| the wrapped standard stream is `stdout` and its buffered content is empty | `stream_flush()` returns `STREAM_STATUS_OK`, leaves the buffered content empty, and leaves the sink content unchanged |
| two consecutive successful flushes are performed on `stdout` | the first flush returns `STREAM_STATUS_OK`, empties the buffered content, and appends it to the sink; the second also returns `STREAM_STATUS_OK` and leaves both the empty buffer and sink content unchanged |
