@page specifications_logger_create logger_create() specifications

# Signature

```c
logger_status_t logger_create(
    logger_t **out,
    const logger_env_t *env);
```

# Purpose

Create a `logger_t` handle by initializing its borrowed fields from the
provided environment.

# Preconditions

- `out` must point to a valid `logger_t *`.
- `env` must point to a valid `logger_env_t`.

# Success

- Returns `LOGGER_STATUS_OK`.
- Stores a newly created `logger_t` handle in `*out`.
- Initializes all borrowed handle fields (i.e. its memory operations and
  virtual table).
- Initializes all owned handle fields to `NULL`.

# Failure

- Returns `LOGGER_STATUS_OOM` on allocation failure.
- Stores `NULL` in `*out`.

# Ownership

- Ownership of `env` and its borrowed dependencies is not transferred.
- On success, ownership of the produced `logger_t` handle is transferred to
  the caller.
- Only the borrowed fields of the produced handle are initialized. Its
  initialization must therefore be completed by
  `logger_complete_default_init()` before the handle can be used.
- The handle must later be released via `logger_destroy()`.

# Notes

- `logger_create()` validates the structural well-formedness of the borrowed
  virtual table.
- This function initializes only the borrowed portion of the handle.
