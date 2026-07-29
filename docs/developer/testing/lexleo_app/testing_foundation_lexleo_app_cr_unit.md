@page testing_foundation_lexleo_app_cr_unit lexleo_app_cr.c unit tests

---

@anchor testing_foundation_lexleo_app_cr_unit_lexleo_app_default_cfg

# lexleo_app_default_cfg() unit tests

See @ref specifications_lexleo_app_default_cfg "lexleo_app_default_cfg() specifications"

## Functions under test

```c
lexleo_app_cfg_t lexleo_app_default_cfg(void);
```

## Test doubles

- None.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `lexleo_app_default_cfg()` is called | returns a `lexleo_app_cfg_t` such that each of `in`, `out`, and `err` has `kind == LEXLEO_APP_IO_STDIO`, `path == NULL`, and `mode == NULL` |

---

@anchor testing_foundation_lexleo_app_cr_unit_lexleo_app_t_lifecycle

# `lexleo_app_t` lifecycle unit tests

See:

- @ref specifications_lexleo_app_create "lexleo_app_create() specifications"
- @ref specifications_lexleo_app_complete_default_init "lexleo_app_complete_default_init() specifications"
- @ref specifications_lexleo_app_destroy "lexleo_app_destroy() specifications"

## Functions under test

```c
lexleo_app_cr_status_t lexleo_app_create(
    lexleo_app_t **out,
    const lexleo_app_env_t *env);

lexleo_app_cr_status_t lexleo_app_complete_default_init(
    lexleo_app_t *app,
    const lexleo_app_cfg_t *cfg);

void lexleo_app_destroy(lexleo_app_t **app);
```

The internal creation and initialization services are called through
`lexleo_app_test_create()` and `lexleo_app_test_complete_default_init()`.
These wrappers translate internal statuses into `lexleo_app_cr_test_status_t`
values.

## Test doubles

- fake memory operations
- fake standard I/O operations
- fake file operations
- fake string operations
- fake time operations
- input, output, error, and logger streams backed by fake stream adapters
- logger backed by a fake logger adapter
- fake VM with link-time substitution of `lexleo_vm_destroy()`
- dummy log path

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `lexleo_app_t` handle fails | `lexleo_app_test_create()` returns `LEXLEO_APP_TEST_STATUS_OOM` and leaves the application handle equal to `NULL` |
| creation succeeds, all runtime resources and the log path are injected by the test infrastructure, and default initialization and destruction are called | `lexleo_app_test_create()` returns `LEXLEO_APP_TEST_STATUS_OK`, stores a non-`NULL` application handle and the supplied memory, standard I/O, file, string, and time operations, and initially stores no streams, logger, or VM and an empty log path; `lexleo_app_test_complete_default_init()` returns `LEXLEO_APP_TEST_STATUS_OK` and preserves the borrowed operations, injected resources, and log path; `lexleo_app_destroy()` sets the application handle to `NULL`, closes each of the four stream backends exactly once, destroys the logger backend exactly once, and calls the VM destructor exactly once with the injected VM handle; the complete lifecycle causes no memory leak, invalid free, or double free |

## Scope

These lifecycle tests exercise the internal creation and initialization
services with injected dependencies. They do not call
`lexleo_app_create_init()` or exercise default runtime resource construction.
