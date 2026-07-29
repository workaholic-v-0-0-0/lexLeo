@page testing_foundation_stdio_stream_cr_unit stdio_stream_cr.c unit tests

---

@anchor testing_foundation_stdio_stream_unit_stdio_stream_vtbl

# `stdio_stream_vtbl()` unit tests

See @ref specifications_stdio_stream_vtbl "stdio_stream_vtbl() specifications"

## Function under test

~~~c
const stream_vtbl_t *stdio_stream_vtbl(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `stdio_stream_vtbl()` is called | returns a non-`NULL` virtual table whose `read`, `write`, `flush`, and `close` operations are all non-`NULL` |

---

@anchor testing_foundation_stdio_stream_unit_stdio_stream_default_cfg

# `stdio_stream_default_cfg()` unit tests

See @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications"

## Function under test

~~~c
stdio_stream_cfg_t stdio_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `stdio_stream_default_cfg()` is called | returns a configuration whose `reserved` field is equal to `0` |

---

@anchor testing_foundation_stdio_stream_unit_stdio_stream_default_env

# `stdio_stream_default_env()` unit tests

See @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications"

## Function under test

~~~c
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- dummy OSAL standard I/O operations pointer
- dummy OSAL memory operations pointer

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `stdio_stream_default_env(stdio_ops, mem_ops)` is called | returns an environment whose `stdio_ops` and `mem_ops` members contain the supplied pointers unchanged |

---

@anchor testing_foundation_stdio_stream_unit_stdio_stream_t_lifecycle

# `stdio_stream_t` lifecycle unit tests

See:

- @ref specifications_stdio_stream_create "stdio_stream_create() specifications"
- @ref specifications_stdio_stream_complete_default_init "stdio_stream_complete_default_init() specifications"
- @ref specifications_stdio_stream_close "stdio_stream_close() specifications"
- @ref specifications_stdio_stream_vtbl "stdio_stream_vtbl() specifications"
- @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications"
- @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications"

## Functions under test

~~~c
stdio_stream_status_t stdio_stream_create(
    stdio_stream_t **out,
    const stdio_stream_env_t *env);

stdio_stream_status_t stdio_stream_complete_default_init(
    stdio_stream_t *stdio_stream,
    const stdio_stream_cfg_t *cfg,
    const stream_standard_stream_creator_args_t *args);
~~~

The backend is destroyed through the `close` operation returned by
`stdio_stream_vtbl()`.

## Test doubles

- fake OSAL standard I/O operations
- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `stdio_stream_t` handle fails | `stdio_stream_create()` returns `STDIO_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| creation of the `stdio_stream_t` handle succeeds | `stdio_stream_create()` returns `STDIO_STREAM_STATUS_OK`, stores a valid non-`NULL` handle in `*out`, preserves the supplied standard I/O and memory operation tables, and initializes the internal borrowed standard stream handle to `NULL` |
| a standard stream handle has already been injected before default initialization | `stdio_stream_complete_default_init()` returns `STDIO_STREAM_STATUS_OK`, preserves the injected standard stream handle, does not invoke any standard stream getter, and preserves the injected operation tables |
| no standard stream handle has been injected before default initialization | `stdio_stream_complete_default_init()` returns `STDIO_STREAM_STATUS_OK`, invokes the getter corresponding to the requested standard stream kind exactly once, stores the returned standard stream handle as an internal borrowed dependency, and preserves the injected operation tables |
| destruction is performed after successful initialization | the `close` operation returns `STREAM_STATUS_OK` and releases the `stdio_stream_t` handle without closing the borrowed standard stream and without memory leak, invalid free, or double free |

---

@anchor testing_foundation_stdio_stream_unit_stream_adapter_provider_t_lifecycle

# `stream_adapter_provider_t` lifecycle unit tests

See:

- @ref specifications_stdio_stream_create_adapter_provider "stdio_stream_create_adapter_provider() specifications"
- @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"

## Functions under test

~~~c
stdio_stream_status_t stdio_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env);

void stream_destroy_adapter_provider(
    stream_adapter_provider_t *provider);
~~~

## Test doubles

- fake OSAL standard I/O operations
- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation required by `stdio_stream_create_adapter_provider()` fails | returns `STDIO_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| adapter provider creation succeeds | returns `STDIO_STREAM_STATUS_OK`, stores a valid non-`NULL` provider in `*out`, initializes a non-`NULL` backend constructor, uses the `stdio_stream` virtual table, stores non-`NULL` constructor user data and destructor, and uses the expected memory operations |
| a successfully created adapter provider is destroyed | `stream_destroy_adapter_provider()` releases all provider-owned resources without memory leak, invalid free, or double free |
