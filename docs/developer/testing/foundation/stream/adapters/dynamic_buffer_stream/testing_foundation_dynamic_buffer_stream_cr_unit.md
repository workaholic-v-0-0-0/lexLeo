@page testing_foundation_dynamic_buffer_stream_cr_unit dynamic_buffer_stream_cr.c unit tests

---

@anchor testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_vtbl

# `dynamic_buffer_stream_vtbl()` unit tests

See @ref specifications_dynamic_buffer_stream_vtbl "dynamic_buffer_stream_vtbl() specifications"

## Function under test

~~~c
const stream_vtbl_t *dynamic_buffer_stream_vtbl(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `dynamic_buffer_stream_vtbl()` is called | returns a non-`NULL` virtual table whose `read`, `write`, `flush`, and `close` operations are all non-`NULL` |

---

@anchor testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_default_cfg

# `dynamic_buffer_stream_default_cfg()` unit tests

See @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications"

## Function under test

~~~c
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `dynamic_buffer_stream_default_cfg()` is called | returns a configuration whose `default_cap` field is greater than `0` |

---

@anchor testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_default_env

# `dynamic_buffer_stream_default_env()` unit tests

See @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications"

## Function under test

~~~c
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
    const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- dummy OSAL memory operations pointer

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `dynamic_buffer_stream_default_env(mem_ops)` is called | returns an environment whose `mem_ops` member contains the supplied pointer unchanged |

---

@anchor testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_t_lifecycle

# `dynamic_buffer_stream_t` lifecycle unit tests

See:

- @ref specifications_dynamic_buffer_stream_create "dynamic_buffer_stream_create() specifications"
- @ref specifications_dynamic_buffer_stream_complete_default_init "dynamic_buffer_stream_complete_default_init() specifications"
- @ref specifications_dynamic_buffer_stream_close "dynamic_buffer_stream_close() specifications"
- @ref specifications_dynamic_buffer_stream_vtbl "dynamic_buffer_stream_vtbl() specifications"
- @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications"
- @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications"

## Functions under test

~~~c
dynamic_buffer_stream_status_t dynamic_buffer_stream_create(
    dynamic_buffer_stream_t **out,
    const dynamic_buffer_stream_env_t *env);

dynamic_buffer_stream_status_t dynamic_buffer_stream_complete_default_init(
    dynamic_buffer_stream_t *dynamic_buffer_stream,
    const dynamic_buffer_stream_cfg_t *cfg);
~~~

The backend is destroyed through the `close` operation returned by
`dynamic_buffer_stream_vtbl()`.

## Test doubles

- fake OSAL memory operations
- white-box test access helpers for internal state inspection and injection

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `dynamic_buffer_stream_t` handle fails | `dynamic_buffer_stream_create()` returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| creation of the `dynamic_buffer_stream_t` handle succeeds | `dynamic_buffer_stream_create()` returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`, stores a valid non-`NULL` handle in `*out`, preserves the supplied memory operation table, and leaves the owned dynamic buffer uninitialized |
| allocation of the owned dynamic buffer fails during default initialization | `dynamic_buffer_stream_complete_default_init()` returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM`, leaves the dynamic buffer uninitialized, preserves the injected memory operations, and causes no memory leak, invalid free, or double free after destruction |
| a valid dynamic buffer state has already been injected before default initialization | `dynamic_buffer_stream_complete_default_init()` returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`, preserves the injected buffer pointer, capacity, length, and read position, and preserves the injected memory operations |
| no dynamic buffer state has been injected and default initialization succeeds | `dynamic_buffer_stream_complete_default_init()` returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`, creates a non-`NULL` owned dynamic buffer with non-zero capacity, initializes its length and read position to `0`, and preserves the injected memory operations |
| destruction is performed after successful initialization | the `close` operation returns `STREAM_STATUS_OK` and releases the owned dynamic buffer and the `dynamic_buffer_stream_t` handle without memory leak, invalid free, or double free |

---

@anchor testing_foundation_dynamic_buffer_stream_unit_stream_adapter_provider_t_lifecycle

# `stream_adapter_provider_t` lifecycle unit tests

See:

- @ref specifications_dynamic_buffer_stream_create_adapter_provider "dynamic_buffer_stream_create_adapter_provider() specifications"
- @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"

## Functions under test

~~~c
dynamic_buffer_stream_status_t dynamic_buffer_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env);

void stream_destroy_adapter_provider(
    stream_adapter_provider_t *provider);
~~~

## Test doubles

- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation required by `dynamic_buffer_stream_create_adapter_provider()` fails | returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| adapter provider creation succeeds | returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`, stores a valid non-`NULL` provider in `*out`, initializes a non-`NULL` backend constructor, uses the `dynamic_buffer_stream` virtual table, stores non-`NULL` constructor user data and destructor, uses the expected memory operations, and `stream_destroy_adapter_provider()` then releases all provider-owned resources without memory leak, invalid free, or double free |
