@page testing_foundation_logger_unit logger unit tests

@anchor testing_foundation_logger_unit_logger_default_env

# logger_default_env() unit tests

See @ref specifications_logger_default_env "logger_default_env() specifications"

## Functions under test

```c
logger_env_t logger_default_env(
    const logger_vtbl_t *vtbl,
    const osal_mem_ops_t *mem_ops);
```

## Test doubles

- dummy `logger_vtbl_t`
- dummy `osal_mem_ops_t`

## Tested scenarios

| WHEN                                                                          | EXPECT                                                                             |
| ----------------------------------------------------------------------------- | ---------------------------------------------------------------------------------- |
| `logger_default_env(vtbl, mem_ops)` is called with the two dummy dependencies | returns a `logger_env_t` such that:<br>`ret.vtbl == vtbl`;<br>`ret.mem == mem_ops` |

## Notes

- This test verifies that the helper stores each injected dependency unchanged
  in the corresponding environment field.
- No ownership or lifetime behavior is exercised by this test.

---

@anchor testing_foundation_logger_unit_logger_create_logger_destroy

# logger_create() / logger_complete_default_init() / logger_destroy() unit tests

See:

- @ref specifications_logger_create "logger_create() specifications"
- @ref specifications_logger_complete_default_init "logger_complete_default_init() specifications"
- @ref specifications_logger_destroy "logger_destroy() specifications"

## Functions under test

```c
logger_status_t logger_create(
    logger_t **out,
    const logger_env_t *env);

logger_status_t logger_complete_default_init(
    logger_t *logger,
    void *backend);

logger_status_t logger_destroy(logger_t **l);
```

## Test doubles

- `fake_memory`
- `fake_logger_backend_t`
- `fake_logger_vtbl`

## Tested scenarios

| WHEN                                                                                                                                                    | EXPECT                                                                                                                                                                                                                                                                                                                                                                 |
| ------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| the nominal logger lifecycle is executed through `logger_create(&out, &env)`, `logger_complete_default_init(out, &backend)`, and `logger_destroy(&out)` | `logger_create()` returns `LOGGER_STATUS_OK`;<br>`out` is non-`NULL`;<br>the newly created logger has no backend attached;<br>`logger_complete_default_init()` returns `LOGGER_STATUS_OK`;<br>the logger backend becomes `&backend`;<br>`logger_destroy()` returns `LOGGER_STATUS_OK`;<br>`out` becomes `NULL`;<br>no memory leak, invalid free, or double free occurs |
| allocation of the logger handle fails during `logger_create(&out, &env)`                                                                                | returns `LOGGER_STATUS_OOM`;<br>`out == NULL`;<br>no memory leak, invalid free, or double free occurs                                                                                                                                                                                                                                                                  |
| the nominal lifecycle is completed and `logger_destroy(&out)` is called twice                                                                           | the first call returns `LOGGER_STATUS_OK`;<br>releases the logger handle;<br>sets `out` to `NULL`;<br>the second call returns `LOGGER_STATUS_OK`;<br>`out` remains `NULL`;<br>the backend destroy operation is invoked exactly once;<br>no memory leak, invalid free, or double free occurs                                                                            |

## Notes

- These tests exercise the public lifecycle of the `logger` port.
- Backend attachment is exercised separately through
  `logger_complete_default_init()`.
- Allocation failure is injected through `fake_memory`.
- The tests verify the absence of memory leaks, invalid frees, and double frees.

---

@anchor testing_foundation_logger_unit_logger_log

# logger_log() unit tests

See @ref specifications_logger_log "logger_log() specifications"

## Functions under test

```c
logger_status_t logger_log(logger_t *l, const char *message);
```

## Test setup

- Unless overridden by the scenario, `l` designates a logger:

    - created by `logger_create()` with `fake_logger_vtbl`;
    - completed by `logger_complete_default_init()` with a
      `fake_logger_backend_t`.
- Unless overridden by the scenario, `message` designates the string
  `"test message"`.

## Test doubles

- `fake_logger_backend_t`
- `fake_logger_vtbl`
- `fake_memory`

## Tested scenarios

| WHEN                                                                                        | EXPECT                                                                                                                                                                                                                             |
| ------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `l == NULL` and `message != NULL`                                                           | returns `LOGGER_STATUS_INVALID`                                                                                                                                                                                                    |
| `l != NULL` and `message == NULL`                                                           | returns `LOGGER_STATUS_INVALID`                                                                                                                                                                                                    |
| `l != NULL`, `message != NULL`, and `fake_logger_backend.log_ret == LOGGER_STATUS_IO_ERROR` | returns `LOGGER_STATUS_IO_ERROR`;<br>calls `fake_logger_vtbl.log` exactly once;<br>passes the injected `fake_logger_backend_t` as the backend argument;<br>passes `message` unchanged;<br>does not call `fake_logger_vtbl.destroy` |

## Notes

- These tests exercise `logger_log()` through a fully initialized logger handle.
- The forwarding scenario verifies that `logger_log()`:

    - delegates to the bound `log` callback;
    - forwards the backend and message arguments unchanged;
    - propagates the callback return status unchanged.
- Logger allocation and destruction performed by the fixture are checked
  through `fake_memory` for leaks, invalid frees, and double frees.

---
