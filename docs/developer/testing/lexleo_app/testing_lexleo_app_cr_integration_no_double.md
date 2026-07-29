@page testing_lexleo_app_cr_integration_no_double lexleo_app_cr.c integration tests without test doubles

---

# `lexleo_app_t` lifecycle integration tests

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
| `lexleo_app_create_init()` is called with the default application configuration, then `lexleo_app_destroy()` is called | creation returns `true` and stores a non-`NULL` application handle; the stored memory, standard I/O, file, string, and time operations match their default providers; input, output, error, and logger streams, the logger, and the VM are non-`NULL`; the log path is nonempty; destruction resets the application handle to `NULL` |

## Scope

This test validates default application composition and destruction with
real components. It does not exercise application execution or stream I/O
behavior.

## Runtime environment

The test accesses the real filesystem during log-path resolution and logger
stream construction. Its environment must permit configuration access,
log-directory preparation, and log-file creation or opening.
