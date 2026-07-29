@page testing_foundation_lexleo_app_cr_unit lexleo_app_cr.c unit tests

---

@anchor testing_foundation_lexleo_app_cr_unit_lexleo_app_default_cfg

# lexleo_app_default_cfg() unit tests

See @ref specifications_lexleo_app_default_cfg "lexleo_app_default_cfg() specifications".

## Functions under test

```c
lexleo_app_cfg_t lexleo_app_default_cfg(void);
```

## Test doubles

- None.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `lexleo_app_default_cfg()` is called | Returns a fully initialized configuration with `action == LEXLEO_APP_ACTION_CONTROL_VM`; every element of `input_paths` and `input_dynamic_libraries` is `NULL`; `output_path == NULL`; each of `in`, `out`, and `err` has `kind == LEXLEO_APP_IO_STDIO`, `path == NULL`, and `mode == NULL`. |

---

@anchor testing_foundation_lexleo_app_cr_unit_lexleo_app_t_lifecycle

# lexleo_app_t lifecycle unit tests

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

## Test doubles and injected test data

- Fake memory operations.
- Fake standard I/O operations.
- Fake file operations.
- Fake string operations.
- Fake time operations.
- Input, output, error, logger, VM input, and VM output streams backed
  by fake stream adapters.
- Logger backed by a fake logger adapter.
- Injected stream factory and standard-stream, regular-file, and
  dynamic-buffer creators.
- Fake VM with link-time substitution of `lexleo_vm_destroy()`.
- Dummy log path copied into the application handle.

The injected VM input and output streams are distinct from the application
input and output streams. Ownership of these distinct streams is transferred
to the application.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| Allocation of the `lexleo_app_t` handle fails, with the output pointer initially NULL | `lexleo_app_test_create()` returns `LEXLEO_APP_TEST_STATUS_OOM` and leaves the output pointer NULL. |
| Creation succeeds | `lexleo_app_test_create()` returns `LEXLEO_APP_TEST_STATUS_OK` and stores a non-NULL application handle. The initial state satisfies the assertions listed below. |
| All runtime resources and a nonempty log path are injected, then initialization is called with the default configuration | `lexleo_app_test_complete_default_init()` returns `LEXLEO_APP_TEST_STATUS_OK`, applies the default configuration, and preserves the injected resources and log path. |
| The initialized application is destroyed | The handle is reset to NULL, each of the six distinct stream backends is closed exactly once, the logger backend is destroyed exactly once, and the VM destructor is called exactly once with the injected VM handle. The lifecycle causes no memory leak, invalid free, or double free. |

### Initial state assertions

After successful creation:

- The handle stores the supplied memory, standard I/O, file, string,
  and time operation pointers.
- Every element of `input_paths` and `input_dynamic_libraries` is NULL.
- `output_path` is NULL.
- `action` is `LEXLEO_APP_ACTION_UNDEFINED`.
- `in`, `out`, `err`, and `logger_stream` are NULL.
- `logger` is NULL.
- The log path is an empty string.
- `stream_owner_opaque` is non-NULL.
- The factory and all three creator fields in that container are NULL.
- `vm_in`, `vm_out`, and `vm` are NULL.

### Initialization assertions

After successful initialization with injected resources:

- The borrowed runtime operation pointers are unchanged.
- Every element of `input_paths` and `input_dynamic_libraries` remains NULL.
- `output_path` remains NULL.
- `action` is `LEXLEO_APP_ACTION_CONTROL_VM`.
- The injected input, output, error, logger, and VM streams are preserved.
- The injected logger and VM are preserved.
- The injected log path is preserved.
- `stream_owner_opaque` remains non-NULL.
- The injected factory and all three creators are preserved.

## Scope

These lifecycle tests exercise the internal creation and initialization
services with injected dependencies. They do not call
`lexleo_app_create_init()` or exercise default runtime resource construction.

The successful lifecycle scenario uses distinct VM input and output streams.
It does not exercise destruction with `vm_in == in` or `vm_out == out`.

The allocation-failure scenario covers failure to allocate the application
handle. It does not cover failure to allocate the stream-owner container.
