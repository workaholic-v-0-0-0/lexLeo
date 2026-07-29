@page specifications_dynamic_buffer_stream_create_init dynamic_buffer_stream_create_init() specifications

# Signature

```c
dynamic_buffer_stream_status_t dynamic_buffer_stream_create_init(
    dynamic_buffer_stream_t **out,
    const dynamic_buffer_stream_env_t *env,
    const dynamic_buffer_stream_cfg_t *cfg
);
```

# Purpose

Create and fully initialize a dynamic-buffer stream backend using a
borrowed runtime environment and a configuration value.

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

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`.
- Stores a valid, fully initialized `dynamic_buffer_stream_t` backend in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the backend according to `cfg`.

# Failure

- Returns the failure status reported by `dynamic_buffer_stream_create()`
  or `dynamic_buffer_stream_complete_default_init()`.
- Leaves `*out` unchanged.

# Ownership

- The created backend is owned by the caller.
- The backend must later be destroyed by `dynamic_buffer_stream_destroy()`
  or transferred to a stream handle.
- Ownership of `env` and `cfg` is not transferred.
- The dependencies stored from `env` are borrowed by the backend.

# Notes

- Combines backend creation and default initialization in one call.
