@page specifications_stream_create_factory stream_create_factory() specifications

# Signature

    stream_status_t stream_create_factory(
        stream_factory_t **out,
        const stream_factory_cfg_t *cfg,
        const stream_env_t *env);

# Purpose

Create a `stream_factory_t` handle from factory configuration and injected
runtime dependencies.

# Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- `env->mem` must not be `NULL`.
- `env->mem->calloc` must not be `NULL`.
- `env->mem->free` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created `stream_factory_t` handle in `*out`.
- The produced factory handle must later be destroyed via
  `stream_destroy_factory()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Notes

- The created factory handle is owned by the caller.
- `stream_destroy_factory()` is the matching destructor for handles created by
  `stream_create_factory()`.
