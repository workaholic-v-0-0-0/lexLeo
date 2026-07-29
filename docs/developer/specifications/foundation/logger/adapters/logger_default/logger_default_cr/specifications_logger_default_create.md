@page specifications_logger_default_create logger_default_create() specifications

# Signature

```c
logger_default_status_t logger_default_create(
    logger_default_t **out,
    const logger_default_env_t *env);
```

# Purpose

Create a partially initialized `logger_default_t` backend from its external
borrowed dependencies.

# Preconditions

- `out` is not `NULL`.
- `env` points to a valid `logger_default_env_t`.

# Success

- Returns `LOGGER_DEFAULT_STATUS_OK`.
- Stores a newly created `logger_default_t` in `*out`.
- Initializes the backend's external borrowed dependencies from `env`,
  including its target stream, time operations, and memory operations.
- Leaves the backend's configuration-dependent state uninitialized.

# Failure

- Returns `LOGGER_DEFAULT_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `env` and the external borrowed dependencies supplied through
  it is not transferred.
- On success, ownership of the produced `logger_default_t` is transferred to
  the caller.
- The produced `logger_default_t` must later be released through the
  `destroy` operation exposed by `logger_default_vtbl()`.

# Notes

- The initialization of the produced `logger_default_t` must be completed by
  `logger_default_complete_default_init()` before it can be used.
- `logger_default_create()` initializes the backend's external borrowed
  dependencies.
- Configuration-dependent state is initialized by
  `logger_default_complete_default_init()`.
- `LOGGER_DEFAULT_STATUS_IO_ERROR` is reserved for operations that may fail
  because of an I/O error and is not produced by this operation.
