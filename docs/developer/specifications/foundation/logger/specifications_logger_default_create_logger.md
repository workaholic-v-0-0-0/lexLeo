@page specifications_logger_default_create_logger logger_default_create_logger() specifications

# Signature

    logger_status_t logger_default_create_logger(
        logger_t **out,
        const logger_default_cfg_t *cfg,
        const logger_default_env_t *env);

# Purpose

Create a logger instance backed by the `logger_default` adapter.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid `logger_default_cfg_t`.
- If `env != NULL`, `env` must point to a valid `logger_default_env_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.

# Success

- Returns `LOGGER_STATUS_OK`.
- Stores a valid newly created logger handle in `*out`.

# Failure

- Returns `LOGGER_STATUS_INVALID` for invalid arguments.
- Returns `LOGGER_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

- On success, ownership of the newly created logger handle is transferred to the
  caller.
- The produced logger handle must later be destroyed via `logger_destroy()`.
- On failure, no logger ownership is transferred.
