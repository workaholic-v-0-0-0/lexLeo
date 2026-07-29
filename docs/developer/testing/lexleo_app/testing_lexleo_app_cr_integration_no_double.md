@page testing_lexleo_app_cr_integration_no_double lexleo_app_cr.c integration tests without test doubles

---

# lexleo_app_t lifecycle integration tests

See:

- @ref specifications_lexleo_app_create_init "lexleo_app_create_init() specifications"
- @ref specifications_lexleo_app_destroy "lexleo_app_destroy() specifications"

## Functions under test

```c
bool lexleo_app_create_init(
    lexleo_app_t **out,
    const lexleo_app_cfg_t *cfg);

void lexleo_app_destroy(lexleo_app_t **app);
```

## Test doubles

- None.

The test uses production runtime operations and concrete application
components. White-box getters observe the composed dependencies and resources.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `lexleo_app_create_init()` is called with the default application configuration | Returns `true` and stores a non-NULL application handle satisfying the composition assertions listed below. |
| `lexleo_app_destroy()` is called on that handle | Destroys the application and resets the handle to NULL. |

### Composition assertions

After successful creation and initialization:

- The stored memory, standard I/O, file, string, and time operations
  match their default providers.
- Every element of `input_paths` and `input_dynamic_libraries` is NULL.
- `output_path` is NULL.
- `action` is `LEXLEO_APP_ACTION_CONTROL_VM`.
- The input, output, and error streams are non-NULL.
- The log path is nonempty.
- The logger stream and logger are non-NULL.
- The `stream_owner_opaque` container is non-NULL.
- The stream factory is non-NULL.
- The standard-stream, regular-file, and dynamic-buffer creators
  are non-NULL.
- `vm_in == in` and `vm_out == out`.
- The VM is non-NULL.

## Scope

This test validates default application composition and destruction with
real components.

It exercises the destruction path where the VM input and output streams
alias the application input and output streams.

It does not execute the VM or explicitly test stream read/write behavior.

The assertions do not independently establish the absence of memory leaks,
invalid accesses, or double frees. Memory-analysis tools can complement
this test.

## Runtime environment

The test accesses the real filesystem during log-path resolution and logger
stream construction. Its environment must permit the configuration access
required by log-path resolution, log-directory preparation, and log-file
creation or opening.

The test may create filesystem entries that are not removed by application
destruction.
