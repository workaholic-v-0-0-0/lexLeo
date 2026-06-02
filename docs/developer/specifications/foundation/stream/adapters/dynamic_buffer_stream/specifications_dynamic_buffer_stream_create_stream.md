@page specifications_dynamic_buffer_stream_create_stream dynamic_buffer_stream_create_stream() specifications

# Signature

```c
stream_status_t dynamic_buffer_stream_create_stream(
    stream_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env);
```

# Purpose

Create a dynamic-buffer-backed `stream_t` handle.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid `dynamic_buffer_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid `dynamic_buffer_stream_env_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a newly created `stream_t` handle in `*out`.
- Creates the dynamic-buffer backend.
- Creates the generic `stream_t` handle from borrowed stream dependencies.
- Completes the stream initialization by transferring backend ownership to the
  stream handle.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged if `out != NULL`.
- Releases any backend created before a later stream-handle creation failure.

# Ownership

- Ownership of `cfg` and `env` is not transferred.
- On success, ownership of the newly created `stream_t` handle is transferred
  to the caller.
- On success, ownership of the dynamic-buffer backend is transferred to the
  produced stream handle.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.
