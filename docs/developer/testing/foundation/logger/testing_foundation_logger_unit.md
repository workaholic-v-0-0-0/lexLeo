@page testing_foundation_logger_unit logger unit tests

@page testing_foundation_logger_unit logger unit tests

It covers:
- `logger_default_env()`
- `logger_create()` / `logger_destroy()`
- `logger_log()`

---

@anchor testing_foundation_logger_unit_logger_default_env
# logger_default_env() unit tests

See @ref specifications_logger_default_env "logger_default_env() specifications"

## Functions under test

~~~c
logger_env_t logger_default_env(const osal_mem_ops_t *mem_ops);
~~~

## Success

- `env.mem == mem_ops`.

## Failure

- None.

## Test doubles

- dummy `osal_mem_ops_t`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `logger_default_env(mem_ops)` is called with a valid `mem_ops` pointer | returns a `logger_env_t` such that `env.mem == mem_ops` |

## Notes

- This helper preserves the allocator dependency provided by the caller.

---

@anchor testing_foundation_logger_unit_logger_create_logger_destroy
# logger_create() / logger_destroy() unit tests

See:
- @ref specifications_logger_create "logger_create() specifications"
- @ref specifications_logger_destroy "logger_destroy() specifications"

## Functions under test

~~~c
logger_status_t logger_create(
    logger_t **out,
    const logger_vtbl_t *vtbl,
    void *backend,
    const logger_env_t *env);

void logger_destroy(logger_t **l);
~~~

## Invalid arguments

- `out`, `vtbl`, `env` must not be `NULL`.
- `backend` must not be `NULL`.
- `vtbl->log`, `vtbl->destroy` must not be `NULL`.
- `env->mem` must not be `NULL`.

## Success

- Returns `LOGGER_STATUS_OK`.
- Stores a valid logger handle in `*out`.
- The produced handle must be destroyed via `logger_destroy()`.

## Failure

- Returns:
    - `LOGGER_STATUS_INVALID` for invalid arguments
    - `LOGGER_STATUS_OOM` on allocation failure
- Leaves `*out` unchanged if `out` is not `NULL`.

## Lifecycle

- `logger_destroy()` does nothing if `l == NULL` or `*l == NULL`.
- Otherwise, it releases the logger object and sets `*l` to `NULL`.

## Test doubles

- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `logger_create(out, vtbl, backend, env)` is called with valid arguments | returns `LOGGER_STATUS_OK`;<br>stores a non-`NULL` logger handle in `*out`;<br>the produced handle is eligible for destruction by `logger_destroy()` |
| `out == NULL` | returns `LOGGER_STATUS_INVALID`;<br>no logger handle is produced |
| `vtbl == NULL` and `out != NULL` | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `vtbl != NULL` but `vtbl->destroy == NULL` and `out != NULL` | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `backend == NULL` and `out != NULL` | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env == NULL` and `out != NULL` | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env != NULL` but `env->mem == NULL` and `out != NULL` | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation of the logger handle fails | returns `LOGGER_STATUS_OOM`;<br>leaves `*out` unchanged |
| `logger_create()` succeeds and `logger_destroy()` is called twice | first `logger_destroy(&l)` releases the handle and sets `l` to `NULL`;<br>second `logger_destroy(&l)` is a no-op and keeps `l` as `NULL` |

## Notes

- These tests validate the public lifecycle contract of the `logger` port.
- The invalid-argument and OOM scenarios verify the output-handle preservation guarantee.
- The idempotent destruction scenario validates the best-effort no-op behavior of `logger_destroy()` when called on an already-destroyed handle.

---

@anchor testing_foundation_logger_unit_logger_log
# logger_log() unit tests

See:
- @ref specifications_logger_log "logger_log() specifications"

## Functions under test

~~~c
logger_status_t logger_log(logger_t *l, const char *message);
~~~

## Precondition

- If `l != NULL`, `l` has been created by `logger_create()` with
  `fake_logger_vtbl` and `fake_logger_backend_t`.

## Invalid arguments

- `l` must not be `NULL`.
- `message` must not be `NULL`.

## Success

- Delegates the log operation to the adapter-facing `log` callback stored in
  the logger handle.
- Returns the value produced by the underlying `log` callback.

## Failure

- If `l == NULL`, returns `LOGGER_STATUS_INVALID`.
- If `message == NULL`, returns `LOGGER_STATUS_INVALID`.

## Test doubles

- `fake_logger_backend_t`
- `fake_logger_vtbl`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `l == NULL` and `message != NULL` | returns `LOGGER_STATUS_INVALID` |
| `l != NULL` and `message == NULL` | returns `LOGGER_STATUS_INVALID` |
| `l != NULL` and `message != NULL` and `fake_logger_backend.log_ret == LOGGER_STATUS_IO_ERROR` | calls `fake_logger_vtbl.log(fake_logger_backend, message)` exactly once;<br>does not call `fake_logger_vtbl.destroy`;<br>returns `LOGGER_STATUS_IO_ERROR` |

## Notes

- These tests validate the borrower-facing runtime contract of the `logger`
  port.
- The forwarding scenario verifies that `logger_log()` delegates to the bound
  adapter callback and propagates its returned status unchanged.

---