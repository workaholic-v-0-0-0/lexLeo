@page testing_foundation_stream_cr_unit stream_cr.c unit tests

---

@anchor testing_foundation_stream_unit_stream_default_env

# stream_default_env() unit tests

See @ref specifications_stream_default_env "stream_default_env() specifications"

## Functions under test

```c
stream_env_t stream_default_env(
    const stream_vtbl_t *vtbl,
    const osal_mem_ops_t *mem_ops);
```

## Test doubles

* dummy `stream_vtbl_t`
* dummy `osal_mem_ops_t`

## Tested scenarios

| WHEN                                                                                   | EXPECT                                                                         |
| -------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| `stream_default_env(vtbl, mem_ops)` is called with valid `vtbl` and `mem_ops` pointers | returns a `stream_env_t` such that `env.vtbl == vtbl` and `env.mem == mem_ops` |

---

@anchor testing_foundation_stream_unit_stream_t_lifecycle

# `stream_t` lifecycle unit tests

See:

- @ref specifications_stream_create "stream_create() specifications"
- @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Functions under test

```c
stream_status_t stream_create(
    stream_t **out,
    const stream_env_t *env);

stream_status_t stream_complete_default_init(
    stream_t *stream,
    void *backend);

void stream_destroy(stream_t **s);
```

## Test doubles

- fake adapter
- fake memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `stream_t` handle fails | `stream_create()` returns `STREAM_STATUS_OOM`, leaves the stream handle unchanged, and causes no memory leak, invalid free, or double free |
| creation succeeds, a backend is injected by the test infrastructure, and `stream_complete_default_init()` is called with `NULL` | `stream_create()` returns `STREAM_STATUS_OK`, stores a non-`NULL` stream handle different from its initial value, stores the supplied virtual table and memory operations, and initially stores no backend; `stream_complete_default_init()` returns `STREAM_STATUS_OK` and preserves the previously injected backend; `stream_destroy()` sets the stream handle to `NULL`, invokes the backend `close` callback exactly once with that backend, and the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds and `stream_complete_default_init()` receives a backend from the production Composition Root | `stream_create()` returns `STREAM_STATUS_OK`, stores a non-`NULL` stream handle different from its initial value, stores the supplied virtual table and memory operations, and initially stores no backend; `stream_complete_default_init()` returns `STREAM_STATUS_OK` and stores the provided backend; `stream_destroy()` sets the stream handle to `NULL`, invokes the backend `close` callback exactly once with that backend, and the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds, a backend is injected by the test infrastructure, and `stream_destroy()` is called twice | the first destruction sets the stream handle to `NULL` and invokes the backend `close` callback exactly once with that backend; the second destruction leaves the stream handle equal to `NULL` and does not invoke the backend `close` callback again; the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds, `stream_complete_default_init()` receives a backend from the production Composition Root, and `stream_destroy()` is called twice | the first destruction sets the stream handle to `NULL` and invokes the backend `close` callback exactly once with that backend; the second destruction leaves the stream handle equal to `NULL` and does not invoke the backend `close` callback again; the complete lifecycle causes no memory leak, invalid free, or double free |

---

@anchor testing_foundation_stream_unit_stream_default_factory_cfg

# stream_default_factory_cfg() unit tests

See @ref specifications_stream_default_factory_cfg "stream_default_factory_cfg() specifications"

## Functions under test

```c
stream_factory_cfg_t stream_default_factory_cfg(void);
```

## Test doubles

None.

## Tested scenarios

| WHEN                                     | EXPECT                                                                   |
| ---------------------------------------- | ------------------------------------------------------------------------ |
| `stream_default_factory_cfg()` is called | returns a `stream_factory_cfg_t` whose `fact_cap` field is equal to `16` |

---

@anchor testing_foundation_stream_unit_stream_factory_t_lifecycle

# `stream_factory_t` lifecycle unit tests

See:

- @ref specifications_stream_create_factory "stream_create_factory() specifications"
- @ref specifications_stream_destroy_factory "stream_destroy_factory() specifications"

## Functions under test

```c
stream_factory_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const osal_mem_ops_t *mem);

void stream_destroy_factory(
    stream_factory_t **fact);
```

## Test doubles

- fake memory operations

## Tested scenarios

| WHEN                                                                                          | EXPECT                                                                                                                                                                                                                                                                                                  |
| --------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| allocation of the `stream_factory_t` handle fails                                             | `stream_create_factory()` returns `STREAM_FACTORY_STATUS_OOM`, leaves `*out` unchanged; subsequent destruction leaves `*out` equal to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free                                                                                   |
| allocation of the backend provider registry fails after the factory handle has been allocated | `stream_create_factory()` returns `STREAM_FACTORY_STATUS_OOM`, releases the previously allocated factory handle, and leaves `*out` unchanged; subsequent destruction leaves `*out` equal to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free                            |
| allocation of both the factory handle and backend provider registry succeeds                  | `stream_create_factory()` returns `STREAM_FACTORY_STATUS_OK` and stores a non-`NULL` factory handle in `*out` different from its initial value; `stream_destroy_factory()` sets `*out` to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free                                  |
| creation succeeds and `stream_destroy_factory()` is called twice                              | the first destruction sets `*out` to `NULL`; the second destruction leaves `*out` equal to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free                                                                                                                            |
