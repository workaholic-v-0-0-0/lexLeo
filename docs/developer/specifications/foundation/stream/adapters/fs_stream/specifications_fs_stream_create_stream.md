@page specifications_fs_stream_create_stream fs_stream_create_stream() specifications

# Signature

    stream_status_t fs_stream_create_stream(
        stream_t **out,
        const fs_stream_args_t *args,
        const fs_stream_cfg_t *cfg,
        const fs_stream_env_t *env);

# Purpose

Create a file-backed stream instance.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid `fs_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid `fs_stream_env_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `args` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- If `args != NULL`, `args->path` must not be `NULL`.
- If `args != NULL`, `args->path` must not be empty.
- If `args != NULL`, `args->flags` must not be zero.

# Success

- Returns `STREAM_STATUS_OK`.
- Opens the underlying OSAL file according to `args`.
- Stores a valid newly created stream handle in `*out`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Returns `STREAM_STATUS_IO_ERROR` if the underlying OSAL file operation fails.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

- On success, ownership of the newly created stream handle is transferred to the
  caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.
