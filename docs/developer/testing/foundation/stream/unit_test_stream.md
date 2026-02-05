@page testing_foundation_stream_unit Stream port unit test specification

This page documents the behavioral contracts verified by the unit tests for the stream port.

It covers:
- `stream_default_ops()`
- `stream_default_env()`
- `stream_create()` / `stream_destroy()`
- `stream_read()`
- `stream_write()`
- `stream_flush()`

# Contract — stream_default_ops()

## Functions under test

```c
const stream_ops_t *stream_default_ops(void);
```

## Success

- Returns a non-`NULL` pointer to a well-formed `stream_ops_t`.
- `read`, `write`, `flush` are non-`NULL`.

## Test doubles

- none

## Tested behaviors

| WHEN | EXPECT |
|---|---|
| `stream_default_ops()` is called | returns a non-`NULL` pointer;<br>`read` is non-`NULL`;<br>`write` is non-`NULL`;<br>`flush` is non-`NULL` |

## Notes

- This function establishes the “non-`NULL` ops pointer implies well-formed” invariant for the stream port.

---

# Contract — stream_default_env()

## Functions under test

```c
stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops);
```

## Success

- `env.mem == mem_ops`.

## Failure

- None.

## Test doubles

- dummy `osal_mem_ops_t`

## Tested behaviors

| WHEN | EXPECT |
|---|---|
| `stream_default_env(mem_ops)` is called with a valid `mem_ops` pointer | returns a `stream_env_t` such that `env.mem == mem_ops` |

## Notes

- This helper preserves the allocator dependency provided by the caller.

---

# Contract — stream_create() / stream_destroy()

## Functions under test

```c
stream_status_t stream_create(
    stream_t **out,
    const stream_vtbl_t *vtbl,
    void *backend,
    const stream_env_t *env);

void stream_destroy(stream_t **s);
```

## Invalid arguments

- `out`, `vtbl`, `env` must not be `NULL`.
- `vtbl->read`, `vtbl->write`, `vtbl->flush`, `vtbl->close` must not be `NULL`.
- `env->mem` must not be `NULL`.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid stream handle in `*out`.
- The produced handle must be destroyed via `stream_destroy()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_OOM` on allocation failure
- Leaves `*out` unchanged if `out` is not `NULL`.

## Lifecycle

- `stream_destroy()` does nothing if `s == NULL` or `*s == NULL`.
- Otherwise, it releases the stream object and sets `*s` to `NULL`.

## Test doubles

- `fake_memory`

## Tested behavior

| WHEN | EXPECT |
|---|---|
| `stream_create(out, vtbl, backend, env)` is called with valid arguments | returns `STREAM_STATUS_OK`;<br>stores a non-NULL stream handle in `*out`;<br>the produced handle is eligible for destruction by `stream_destroy()` |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `vtbl == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env != NULL` but `env->mem == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation of the stream handle fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |
| `stream_create()` succeeds and `stream_destroy()` is called twice | first `stream_destroy(&s)` releases the handle and sets `s` to `NULL`;<br>second `stream_destroy(&s)` is a no-op and keeps `s` as `NULL` |
| `vtbl != NULL` but `vtbl->read == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |

---

# Contract — stream_read()

## Functions under test

```c
size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
```

## Precondition

- If `s != NULL`, `s` has been created by `stream_create()` with `fake_stream_vtbl` and `fake_stream_backend`.

## Test doubles

- `fake_stream_backend_t`
- `fake_stream_vtbl`

## Tested behaviors

| WHEN | EXPECT |
|---|---|
| `n == 0` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_OK`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n == 0` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_INVALID`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `buf == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_INVALID`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `buf == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_NO_BACKEND`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st != NULL`, with `fake_stream_backend` configured as `read_ret = n` and `read_st_to_set = STREAM_STATUS_OK` | calls `fake_stream_vtbl.read(fake_stream_backend, buf, n, st)` exactly once;<br>does not call `fake_stream_vtbl.write/flush/close`;<br>returns `n`;<br>sets `*st = STREAM_STATUS_OK` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st != NULL`, with `fake_stream_backend` configured as `read_ret = 0` and `read_st_to_set = STREAM_STATUS_EOF` | calls `fake_stream_vtbl.read(fake_stream_backend, buf, n, st)` exactly once;<br>does not call `fake_stream_vtbl.write/flush/close`;<br>returns `0`;<br>sets `*st = STREAM_STATUS_EOF` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st == NULL`, with `fake_stream_backend` configured as `read_ret = 5` | calls `fake_stream_vtbl.read(fake_stream_backend, buf, n, st = NULL)` exactly once;<br>does not call `fake_stream_vtbl.write/flush/close`;<br>returns `5` |

## Notes

- For `n == 0`, the operation succeeds immediately without consulting the backend.
- When `st == NULL`, status propagation is omitted but return-value and forwarding behavior remain verified.
- The nominal scenarios validate borrower forwarding to the backend `read` operation and the absence of unrelated backend calls.

---

# Contract — stream_write()

## Functions under test

```c
size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);
```

## Precondition

- If `s != NULL`, `s` has been created by `stream_create()` with `fake_stream_vtbl` and `fake_stream_backend`.

## Test doubles

- `fake_stream_backend_t`
- `fake_stream_vtbl`

## Tested behaviors

| WHEN | EXPECT |
|---|---|
| `n == 0` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_OK`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n == 0` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_INVALID`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `buf == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_INVALID`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `buf == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend == NULL` and `st != NULL` | returns `0`;<br>sets `*st = STREAM_STATUS_NO_BACKEND`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend == NULL` and `st == NULL` | returns `0`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st != NULL`, with `fake_stream_backend` configured as `write_ret = n` and `write_st_to_set = STREAM_STATUS_OK` | calls `fake_stream_vtbl.write(fake_stream_backend, buf, n, st)` exactly once;<br>does not call `fake_stream_vtbl.read/flush/close`;<br>returns `n`;<br>sets `*st = STREAM_STATUS_OK` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st != NULL`, with `fake_stream_backend` configured as `write_ret = 0` and `write_st_to_set = STREAM_STATUS_IO_ERROR` | calls `fake_stream_vtbl.write(fake_stream_backend, buf, n, st)` exactly once;<br>does not call `fake_stream_vtbl.read/flush/close`;<br>returns `0`;<br>sets `*st = STREAM_STATUS_IO_ERROR` |
| `n > 0` and `s != NULL` and `buf != NULL` and `s->backend != NULL` and `st == NULL`, with `fake_stream_backend` configured as `write_ret = 5` | calls `fake_stream_vtbl.write(fake_stream_backend, buf, n, st = NULL)` exactly once;<br>does not call `fake_stream_vtbl.read/flush/close`;<br>returns `5` |

## Notes

- For `n == 0`, the operation succeeds immediately without consulting the backend.
- When `st == NULL`, status propagation is omitted but return-value and forwarding behavior remain verified.
- The nominal scenarios validate borrower forwarding to the backend `write` operation and the absence of unrelated backend calls.

---

# Contract — stream_flush()

## Functions under test

```c
stream_status_t stream_flush(stream_t *s);
```

## Precondition

- If `s != NULL`, `s` has been created by `stream_create()` with `fake_stream_vtbl` and `fake_stream_backend`.

## Test doubles

- `fake_stream_backend_t`
- `fake_stream_vtbl`

## Tested behaviors

| WHEN | EXPECT |
|---|---|
| `s == NULL` | returns `STREAM_STATUS_INVALID`;<br>does not call `fake_stream_vtbl.read/write/flush/close` |
| `s != NULL` and `s->backend == NULL` | returns `STREAM_STATUS_NO_BACKEND`;<br>does not call `fake_stream_vtbl.flush` |
| `s != NULL` and `s->backend != NULL`, with `fake_stream_backend` configured as `flush_ret = STREAM_STATUS_OK` | returns `STREAM_STATUS_OK`;<br>calls `fake_stream_vtbl.flush(fake_stream_backend)` exactly once;<br>does not call `fake_stream_vtbl.read/write/close` |
| `s != NULL` and `s->backend != NULL`, with `fake_stream_backend` configured as `flush_ret = STREAM_STATUS_IO_ERROR` | returns `STREAM_STATUS_IO_ERROR`;<br>calls `fake_stream_vtbl.flush(fake_stream_backend)` exactly once;<br>does not call `fake_stream_vtbl.read/write/close` |

## Notes

- The nominal scenarios validate borrower forwarding to the backend `flush` operation and the absence of unrelated backend calls.
- The `backend == NULL` case verifies that the port reports the missing backend without attempting any flush operation.
