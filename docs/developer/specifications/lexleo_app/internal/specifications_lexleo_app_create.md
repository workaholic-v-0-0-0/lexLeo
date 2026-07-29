@page specifications_lexleo_app_create lexleo_app_create() specifications

# Signature

```c
lexleo_app_cr_status_t lexleo_app_create(
    lexleo_app_t **out,
    const lexleo_app_env_t *env
);
```

# Purpose

Create a LexLeo application handle from a borrowed runtime environment,
ready for subsequent configuration and component initialization.

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
- Initializes every element of `input_paths` and
  `input_dynamic_libraries` to `NULL`.
- Initializes `output_path` to `NULL`.
- Initializes `action` to `LEXLEO_APP_ACTION_UNDEFINED`.
- Initializes `in`, `out`, `err`, and `logger_stream` to `NULL`.
- Initializes `logger` to `NULL`.
- Initializes the log path to an empty string.
- Allocates the container designated by `stream_owner_opaque`.
- Initializes the stream factory and all stream creator fields
  in that container to `NULL`.
- Initializes `vm_in`, `vm_out`, and `vm` to `NULL`.
- Does not perform the initialization completed later by
  `lexleo_app_complete_default_init()`.

# Failure

- Returns `LEXLEO_APP_CR_STATUS_OOM` if allocation of the handle
  or its stream owner container fails.
- Releases any resources allocated during the call.
- Leaves `*out` unchanged.
- Does not release the borrowed dependencies provided by `env`.

# Ownership

- The created application handle is owned by the caller.
- The handle must later be destroyed by `lexleo_app_destroy()`,
  including if subsequent initialization is not performed or fails.
- The application owns the container designated by
  `stream_owner_opaque`.
- The dependencies stored from `env` are borrowed by the application
  and must remain valid until the application is destroyed.
- The `env` structure itself is not retained.

# Notes

- Application configuration is supplied later to
  `lexleo_app_complete_default_init()`.
- This function does not create streams, stream factories,
  stream creators, a logger, or a VM.
- The newly created handle is not yet ready for `lexleo_app_run()`.
