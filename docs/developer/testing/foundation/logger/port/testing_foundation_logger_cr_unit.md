@page testing_foundation_logger_cr_unit logger_cr.c unit tests

---

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

| WHEN                                                                                   | EXPECT                                                                                 |
| -------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------- |
| `logger_default_env(vtbl, mem_ops)` is called with valid `vtbl` and `mem_ops` pointers | returns a `logger_env_t` such that `env.vtbl == vtbl` and `env.mem_ops == mem_ops`     |

---

@anchor testing_foundation_logger_unit_logger_t_lifecycle

# `logger_t` lifecycle unit tests

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

logger_status_t logger_destroy(
    logger_t **logger);
```

## Test doubles

- fake adapter
- fake memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `logger_t` handle fails | `logger_create()` returns `LOGGER_STATUS_OOM`, leaves the logger handle unchanged, and causes no memory leak, invalid free, or double free |
| creation succeeds, a backend is injected by the test infrastructure, and `logger_complete_default_init()` is called with `NULL` | `logger_create()` returns `LOGGER_STATUS_OK`, stores a non-`NULL` logger handle different from its initial value, stores the supplied virtual table and memory operations, and initially stores no backend; `logger_complete_default_init()` returns `LOGGER_STATUS_OK` and preserves the previously injected backend; `logger_destroy()` sets the logger handle to `NULL`, invokes the backend `destroy` callback exactly once with that backend, and the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds and `logger_complete_default_init()` receives a backend from the production Composition Root | `logger_create()` returns `LOGGER_STATUS_OK`, stores a non-`NULL` logger handle different from its initial value, stores the supplied virtual table and memory operations, and initially stores no backend; `logger_complete_default_init()` returns `LOGGER_STATUS_OK` and stores the provided backend; `logger_destroy()` sets the logger handle to `NULL`, invokes the backend `destroy` callback exactly once with that backend, and the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds, a backend is injected by the test infrastructure, and `logger_destroy()` is called twice | the first destruction sets the logger handle to `NULL` and invokes the backend `destroy` callback exactly once with that backend; the second destruction leaves the logger handle equal to `NULL` and does not invoke the backend `destroy` callback again; the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds, `logger_complete_default_init()` receives a backend from the production Composition Root, and `logger_destroy()` is called twice | the first destruction sets the logger handle to `NULL` and invokes the backend `destroy` callback exactly once with that backend; the second destruction leaves the logger handle equal to `NULL` and does not invoke the backend `destroy` callback again; the complete lifecycle causes no memory leak, invalid free, or double free |
