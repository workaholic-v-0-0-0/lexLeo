@page specifications_lexleo_app_create_init lexleo_app_create_init() specifications

# Signature

```c
bool lexleo_app_create_init(
    lexleo_app_t **out,
    const lexleo_app_cfg_t *cfg
);
```

# Purpose

Act as the LexLeo application Composition Root by creating and fully
initializing the application using `cfg` and the default runtime
environment.

# Preconditions

- `out != NULL`.
- `cfg != NULL`.
- `cfg` must be supported:
  - For each of `cfg->in`, `cfg->out`, and `cfg->err`:
    - `kind` must be `LEXLEO_APP_IO_STDIO`, `LEXLEO_APP_IO_FILE`,
      or `LEXLEO_APP_IO_BUFFER`.
    - For `LEXLEO_APP_IO_FILE`, `path` must point to a valid,
      null-terminated file path and `mode` must be supported for
      the corresponding stream.

# Success

- Returns `true`.
- Stores a valid, fully initialized `lexleo_app_t` handle in `*out`.
- Composes the application components with their runtime dependencies.

# Failure

- Returns `false` if creation or initialization fails.
- Leaves `*out` unchanged.

# Ownership

- The created application handle is owned by the caller.
- The handle must later be destroyed by `lexleo_app_destroy()`.
- Ownership of `cfg` is not transferred.

# Notes

- Combines application creation and default initialization in one call.
- Detailed internal status codes are not exposed through this function.
