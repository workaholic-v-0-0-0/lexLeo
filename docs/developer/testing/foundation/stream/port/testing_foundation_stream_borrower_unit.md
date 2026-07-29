@page testing_foundation_stream_borrower_unit stream_borrower.c unit tests

---

@anchor testing_foundation_stream_unit_stream_read

# `stream_read()` unit tests

See @ref specifications_stream_read "stream_read() specifications"

## Functions under test

```c
size_t stream_read(
    stream_t *s,
    void *buf,
    size_t n,
    stream_status_t *st);
```

## Test doubles

- fake stream adapter

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `stream_read(NULL, buf, n, st)` is called with `n > 0` | returns `0`, sets `*st` to `STREAM_STATUS_INVALID`, and does not invoke the backend `read()` operation |
| `stream_read(s, NULL, n, st)` is called with `n > 0` | returns `0`, sets `*st` to `STREAM_STATUS_INVALID`, and does not invoke the backend `read()` operation |
| `stream_read(s, buf, 0, NULL)` is called | returns `0` and does not invoke the backend `read()` operation |
| `stream_read(s, buf, 0, st)` is called | returns `0`, sets `*st` to `STREAM_STATUS_OK`, and does not invoke the backend `read()` operation |
| the backend `read()` operation returns `STREAM_STATUS_IO_ERROR` and a byte count | invokes the backend `read()` operation exactly once, forwards the backend handle, buffer pointer, byte count, and status pointer unchanged, returns the backend byte count unchanged, and stores `STREAM_STATUS_IO_ERROR` in `*st` |
| the backend `read()` operation returns `STREAM_STATUS_EOF` and a byte count | invokes the backend `read()` operation exactly once, forwards the backend handle, buffer pointer, byte count, and status pointer unchanged, returns the backend byte count unchanged, and stores `STREAM_STATUS_EOF` in `*st` |
| the backend `read()` operation returns `STREAM_STATUS_OK` and a byte count | invokes the backend `read()` operation exactly once, forwards the backend handle, buffer pointer, byte count, and status pointer unchanged, returns the backend byte count unchanged, and stores `STREAM_STATUS_OK` in `*st` |

---

@anchor testing_foundation_stream_unit_stream_write

# `stream_write()` unit tests

See:

- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stream_create "stream_create() specifications"
- @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Function under test

```c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
```

## Test doubles

- fake stream adapter

## Tested scenarios

| WHEN                                                          | EXPECT                                                                                                                                                                                                |
| ------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `n > 0` and `s == NULL`                                       | `stream_write()` returns `0`; if `st != NULL`, sets `*st = STREAM_STATUS_INVALID`; no backend write operation is performed                                                                            |
| `n > 0` and `buf == NULL`                                     | `stream_write()` returns `0`; if `st != NULL`, sets `*st = STREAM_STATUS_INVALID`; no backend write operation is performed                                                                            |
| `n == 0` and `st == NULL`                                     | `stream_write()` returns `0`; no backend write operation is performed                                                                                                                                |
| `n == 0` and `st != NULL`                                     | `stream_write()` returns `0`; sets `*st = STREAM_STATUS_OK`; no backend write operation is performed                                                                                                 |
| the backend write operation produces `STREAM_STATUS_IO_ERROR` | `stream_write()` delegates the operation to the backend, returns the value produced by the backend write operation, and, if `st != NULL`, stores `STREAM_STATUS_IO_ERROR` in `*st`                 |
| the backend write operation produces `STREAM_STATUS_OK`       | `stream_write()` delegates the operation to the backend, passes the expected backend, buffer, byte count, and status pointer, returns the value produced by the backend write operation, and, if `st != NULL`, stores `STREAM_STATUS_OK` in `*st` |

---

@anchor testing_foundation_stream_unit_stream_flush

# `stream_flush()` unit tests

See:

- @ref specifications_stream_flush "stream_flush() specifications"
- @ref specifications_stream_create "stream_create() specifications"
- @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Function under test

```c
stream_status_t stream_flush(stream_t *s);
```

## Test doubles

- fake stream adapter backend

## Tested scenarios

| WHEN                                                        | EXPECT                                                                                                                                                                                 |
| ----------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `s == NULL`                                                 | `stream_flush()` returns `STREAM_STATUS_INVALID`; no backend flush operation is performed                                                                                             |
| the backend flush operation returns `STREAM_STATUS_IO_ERROR` | `stream_flush()` delegates the operation to the backend, passes the expected backend, and returns `STREAM_STATUS_IO_ERROR`                                                           |
| the backend flush operation returns `STREAM_STATUS_OK`       | `stream_flush()` delegates the operation to the backend, passes the expected backend, and returns `STREAM_STATUS_OK`                                                                 |
