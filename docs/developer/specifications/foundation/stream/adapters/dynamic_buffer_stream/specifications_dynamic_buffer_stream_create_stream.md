@page specifications_dynamic_buffer_stream_create_stream dynamic_buffer_stream_create_stream() specifications

# Signature

```c
stream_status_t dynamic_buffer_stream_create_stream(
    stream_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env);
```

# Purpose

Create a `dynamic_buffer_stream` instance directly.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid
  `dynamic_buffer_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid
  `dynamic_buffer_stream_env_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created stream handle in `*out`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

- On success, ownership of the newly created stream handle is transferred to the
  caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.
