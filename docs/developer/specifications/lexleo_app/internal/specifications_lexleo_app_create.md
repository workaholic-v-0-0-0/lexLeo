@page specifications_lexleo_app_create lexleo_app_create() specifications

# Signature

```c
lexleo_app_cr_status_t lexleo_app_create(
    lexleo_app_t **out,
    const lexleo_app_env_t *env
);
```

# Purpose

Create a LexLeo application handle from a borrowed runtime environment.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LEXLEO_APP_CR_STATUS_OK`.
- Stores a valid newly created `lexleo_app_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the input, output, error, and logger stream fields to `NULL`.
- Initializes the logger and VM fields to `NULL`.
- Initializes the log path to an empty string.
- Does not perform the initialization completed later by
  `lexleo_app_complete_default_init()`.

# Failure

- Returns `LEXLEO_APP_CR_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- The created application handle is owned by the caller.
- The handle must later be destroyed by `lexleo_app_destroy()`.
- The dependencies stored from `env` are borrowed by the application.

# Notes

- Application configuration is supplied later to
  `lexleo_app_complete_default_init()`.
