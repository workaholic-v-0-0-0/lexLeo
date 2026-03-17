@page specifications_logger_create logger_create() specifications

# Signature

```c
logger_status_t logger_create(
    logger_t **out,
    const logger_vtbl_t *vtbl,
    void *backend,
    const logger_env_t *env);
```

# Purpose

Create the public `logger_t` handle of the `logger` port from an adapter-facing
virtual table, a backend pointer, and injected runtime dependencies.

# Invalid arguments

- `out` must not be `NULL`.
- `vtbl` must not be `NULL`.
- `env` must not be `NULL`.
- `backend` must not be `NULL`.
- `env->mem` must not be `NULL`.
- `vtbl->log` must not be `NULL`.
- `vtbl->destroy` must not be `NULL`.

# Success

- Returns `LOGGER_STATUS_OK`.
- Stores a valid newly created `logger_t` handle in `*out`.
- The produced logger handle must later be destroyed via `logger_destroy()`.

# Failure

- Returns `LOGGER_STATUS_INVALID` for invalid arguments.
- Returns `LOGGER_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Notes

- `logger_create()` validates the structural well-formedness of the adapter-
  facing virtual table at creation time.
- The `backend` pointer is stored as provided in the created logger handle.
- The created handle is owned by the caller.
- `logger_destroy()` is the matching destructor for handles created by
  `logger_create()`.
