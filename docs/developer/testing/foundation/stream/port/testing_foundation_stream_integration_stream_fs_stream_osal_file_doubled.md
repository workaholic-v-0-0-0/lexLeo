@page testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled stream / fs_stream integration tests with doubled OSAL file dependency

---

@anchor testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_smoke

# `stream` / `fs_stream` smoke tests

See:

- @ref specifications_fs_stream_create_adapter_provider "fs_stream_create_adapter_provider() specifications"
- @ref specifications_stream_create_factory "stream_create_factory() specifications"
- @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications"
- @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications"
- @ref specifications_stream_regular_file_creator_create "stream_regular_file_creator_create() specifications"
- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Test doubles

- fake OSAL file dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| an `fs_stream` adapter provider is registered in a stream factory and a regular-file stream is created through it | creation succeeds, the OSAL file open operation is invoked with the expected path, mode, and memory operations, the created stream reads the expected file content through `fs_stream`, and destroying the stream closes the underlying OSAL file |

---

@anchor testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_read

# `stream_read()` / `fs_stream` integration tests

See:

- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_fs_stream_read "fs_stream_read() specifications"

## Function under test

~~~c
size_t stream_read(
    stream_t *s,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Test doubles

- fake OSAL file dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `n > 0` and `s == NULL` | `stream_read()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the destination buffer unchanged |
| `n > 0` and `buf == NULL` | `stream_read()` returns `0` and sets `*st = STREAM_STATUS_INVALID` |
| the OSAL file read operation produces `OSAL_FILE_STATUS_IO` | `stream_read()` returns `0`, maps the OSAL file status to `STREAM_STATUS_IO_ERROR`, and leaves the destination buffer unchanged |
| `n == 0` and `s == NULL` | `stream_read()` returns `0`, sets `*st = STREAM_STATUS_OK`, and leaves the destination buffer unchanged |
| `n` exceeds the number of bytes remaining in the file | `stream_read()` returns the number of remaining bytes, copies those bytes to the destination buffer, and reports `STREAM_STATUS_EOF` |
| `n` exactly matches the number of bytes remaining in the file | `stream_read()` returns `n`, copies the remaining bytes to the destination buffer, and reports `STREAM_STATUS_OK` |
| the file position is already at EOF | `stream_read()` returns `0`, reports `STREAM_STATUS_EOF`, and leaves the destination buffer unchanged |
| fewer than the remaining bytes are requested | `stream_read()` returns `n`, copies the requested bytes to the destination buffer, and reports `STREAM_STATUS_OK` |
| two reads are performed and data remains after the second read | both reads return the requested byte count and report `STREAM_STATUS_OK`; the second read continues from the position reached by the first |
| two reads consume exactly all remaining data | both reads return the requested byte count and report `STREAM_STATUS_OK`; the second read reaches EOF without reporting `STREAM_STATUS_EOF` |
| the second of two reads requests more bytes than remain | the first read reports `STREAM_STATUS_OK`; the second returns the number of remaining bytes and reports `STREAM_STATUS_EOF` |
| the first read consumes all remaining data and a second read is attempted | the first read returns the requested byte count and reports `STREAM_STATUS_OK`; the second returns `0` and reports `STREAM_STATUS_EOF` |

---

@anchor testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_write

# `stream_write()` / `fs_stream` integration tests

See:

- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_fs_stream_write "fs_stream_write() specifications"

## Function under test

~~~c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Test doubles

- fake OSAL file dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `n > 0` and `s == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the fake file buffered content unchanged |
| `n > 0` and `buf == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_INVALID`, and leaves the fake file buffered content unchanged |
| the OSAL file write operation produces `OSAL_FILE_STATUS_IO` | `stream_write()` returns `0`, maps the OSAL file status to `STREAM_STATUS_IO_ERROR`, and leaves the fake file buffered content unchanged |
| `n == 0` and `s == NULL` | `stream_write()` returns `0`, sets `*st = STREAM_STATUS_OK`, and leaves the fake file buffered content unchanged |
| one write fits within the existing buffered file content | `stream_write()` returns the requested byte count, reports `STREAM_STATUS_OK`, and writes the supplied bytes at the current file position |
| one write extends beyond the existing buffered file content | `stream_write()` returns the requested byte count, reports `STREAM_STATUS_OK`, writes the supplied bytes at the current file position, and extends the buffered file length accordingly |
| two consecutive writes are performed | both writes return their requested byte counts and report `STREAM_STATUS_OK`; the second write continues from the position reached by the first |

---

@anchor testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_flush

# `stream_flush()` / `fs_stream` integration tests

See:

- @ref specifications_stream_flush "stream_flush() specifications"
- @ref specifications_fs_stream_flush "fs_stream_flush() specifications"

## Function under test

~~~c
stream_status_t stream_flush(stream_t *s);
~~~

## Test doubles

- fake OSAL file dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `s == NULL` | `stream_flush()` returns `STREAM_STATUS_INVALID`, does not invoke the OSAL file flush operation, and leaves the buffered and sink contents unchanged |
| the OSAL file flush operation produces `OSAL_FILE_STATUS_IO` | the OSAL file flush operation is invoked exactly once, `stream_flush()` maps the status to `STREAM_STATUS_IO_ERROR`, and the buffered and sink contents remain unchanged |
| the OSAL file flush operation produces `OSAL_FILE_STATUS_OK` | the OSAL file flush operation is invoked exactly once, `stream_flush()` returns `STREAM_STATUS_OK`, the buffered content remains unchanged, and the sink content contains the flushed buffered data |
