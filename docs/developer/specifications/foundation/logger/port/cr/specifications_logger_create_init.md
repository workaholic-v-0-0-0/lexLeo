@page specifications_logger_create_init logger_create_init() specifications

# Signature

```c
logger_status_t logger_create_init(
    logger_t **out,
    const logger_env_t *env,
    void *backend
);
```

# Purpose

Create and fully initialize a logger handle using a borrowed runtime
environment and an adapter backend.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `backend` must be a valid adapter backend compatible with `env`.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LOGGER_STATUS_OK`.
- Stores a valid, fully initialized `logger_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Binds `backend` to the created logger.

# Failure

- Returns the failure status reported by `logger_create()` or
  `logger_complete_default_init()`.
- Leaves `*out` unchanged.
- Ownership of `backend` remains with the caller.

# Ownership

- The created logger handle is owned by the caller.
- The handle must later be destroyed by `logger_destroy()`.
- On success, ownership of `backend` is transferred to the logger.
- Ownership of `env` is not transferred.
- The dependencies stored from `env` are borrowed by the logger.

# Notes

- Combines logger creation and default initialization in one call.
- The adapter backend is created outside the `logger` port.
