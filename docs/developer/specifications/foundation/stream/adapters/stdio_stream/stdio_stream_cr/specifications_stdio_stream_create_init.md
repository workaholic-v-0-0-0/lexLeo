@page specifications_stdio_stream_create_init stdio_stream_create_init() specifications

# Signature

```c
stdio_stream_status_t stdio_stream_create_init(
    stdio_stream_t **out,
    const stdio_stream_env_t *env,
    const stdio_stream_cfg_t *cfg,
    const stream_standard_stream_kind_t kind
);
```

# Purpose

Create and fully initialize a standard-stream backend using a borrowed
runtime environment, a configuration value, and a standard-stream kind.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `cfg != NULL`.
- `cfg` must be supported.
- `cfg` must be well-formed.
- `kind` must be a standard-stream kind supported by
  `stdio_stream_complete_default_init()`.
- `*out` does not need to be initialized before the call.

# Success

- Returns `STDIO_STREAM_STATUS_OK`.
- Stores a valid, fully initialized `stdio_stream_t` backend in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the backend according to `cfg` and `kind`.

# Failure

- Returns the failure status reported by `stdio_stream_create()` or
  `stdio_stream_complete_default_init()`.
- Leaves `*out` unchanged.

# Ownership

- The created backend is owned by the caller.
- The backend must later be released through `stdio_stream_vtbl()->close()`
  or transferred to a stream handle.
- Ownership of `env` and `cfg` is not transferred.
- The dependencies stored from `env` are borrowed by the backend.

# Notes

- Combines backend creation and default initialization in one call.
