@page testing_foundation_logger_default_cr_unit logger_default_cr.c unit tests

---

@anchor testing_foundation_logger_default_unit_logger_default_vtbl

# `logger_default_vtbl()` unit tests

See @ref specifications_logger_default_vtbl "logger_default_vtbl() specifications"

## Function under test

~~~c
const logger_vtbl_t *logger_default_vtbl(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `logger_default_vtbl()` is called | returns a non-`NULL` virtual table whose `log` and `destroy` operations are both non-`NULL` |

---

@anchor testing_foundation_logger_default_unit_logger_default_default_cfg

# `logger_default_default_cfg()` unit tests

See @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications"

## Function under test

~~~c
logger_default_cfg_t logger_default_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `logger_default_default_cfg()` is called | returns a configuration whose `append_newline` field is `true` |

---

@anchor testing_foundation_logger_default_unit_logger_default_default_env

# `logger_default_default_env()` unit tests

See @ref specifications_logger_default_default_env "logger_default_default_env() specifications"

## Function under test

~~~c
logger_default_env_t logger_default_default_env(
    stream_t *stream,
    const osal_time_ops_t *time_ops,
    const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- dummy stream pointer
- dummy OSAL time operations pointer
- dummy OSAL memory operations pointer

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `logger_default_default_env(stream, time_ops, mem_ops)` is called | returns an environment whose `stream`, `time_ops`, and `mem_ops` members contain the supplied pointers unchanged |

---

@anchor testing_foundation_logger_default_unit_logger_default_t_lifecycle

# `logger_default_t` lifecycle unit tests

See:

- @ref specifications_logger_default_create "logger_default_create() specifications"
- @ref specifications_logger_default_complete_default_init "logger_default_complete_default_init() specifications"
- @ref specifications_logger_default_destroy "logger_default_destroy() specifications"
- @ref specifications_logger_default_vtbl "logger_default_vtbl() specifications"
- @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications"
- @ref specifications_logger_default_default_env "logger_default_default_env() specifications"

## Functions under test

~~~c
logger_default_status_t logger_default_create(
    logger_default_t **out,
    const logger_default_env_t *env);

logger_default_status_t logger_default_complete_default_init(
    logger_default_t *logger_default,
    const logger_default_cfg_t *cfg);
~~~

The backend is destroyed through the `destroy` operation returned by
`logger_default_vtbl()`.

## Test doubles

- fake stream
- fake OSAL time operations
- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `logger_default_t` handle fails | `logger_default_create()` returns `LOGGER_DEFAULT_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| creation of the `logger_default_t` handle succeeds | `logger_default_create()` returns `LOGGER_DEFAULT_STATUS_OK`, stores a valid non-`NULL` handle in `*out`, and preserves the supplied stream, time operations, and memory operations |
| default initialization is completed after successful creation | `logger_default_complete_default_init()` returns `LOGGER_DEFAULT_STATUS_OK`, preserves the injected stream, time operations, and memory operations, and initializes `append_newline` from the default configuration to `true` |
| destruction is performed after successful initialization | the `destroy` operation returned by `logger_default_vtbl()` returns `LOGGER_STATUS_OK`, releases the `logger_default_t` handle, and causes no memory leak, invalid free, or double free |
