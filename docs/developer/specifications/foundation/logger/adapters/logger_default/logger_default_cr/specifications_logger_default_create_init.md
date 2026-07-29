@page specifications_logger_default_create_init logger_default_create_init() specifications

# Signature

```c
logger_default_status_t logger_default_create_init(
    logger_default_t **out,
    const logger_default_env_t *env,
    const logger_default_cfg_t *cfg
);
```

# Purpose

Create and fully initialize a logger backend using a borrowed runtime
environment and a configuration value.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `cfg != NULL`.
- `cfg` must be supported.
- `cfg` must be well-formed.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LOGGER_DEFAULT_STATUS_OK`.
- Stores a valid, fully initialized `logger_default_t` backend in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the backend according to `cfg`.

# Failure

- Returns the failure status reported by `logger_default_create()` or
  `logger_default_complete_default_init()`.
- Leaves `*out` unchanged.

# Ownership

- The created backend is owned by the caller.
- The backend must later be released through `logger_default_vtbl()->destroy()`
  or transferred to a logger handle.
- Ownership of `env` and `cfg` is not transferred.
- The dependencies stored from `env`, including the target stream, are
  borrowed by the backend.

# Notes

- Combines backend creation and default initialization in one call.
