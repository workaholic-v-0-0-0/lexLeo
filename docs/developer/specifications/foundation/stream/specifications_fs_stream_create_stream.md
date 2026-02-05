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

- `args` must designate a valid `fs_stream_args_t`.
- `cfg` must designate a valid `fs_stream_cfg_t`.
- `env` must designate a valid `fs_stream_env_t`.
- `env` must provide the injected dependencies required to open and manage the
  underlying OSAL file.

# Invalid arguments

- `out` must not be `NULL`.
- `args` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- `args->path` must not be `NULL`.
- `args->path` must not be empty.
- `args->flags` must not be zero.

# Success

- Returns `STREAM_STATUS_OK`.
- Allocates and initializes a public `stream_t`.
- Opens the underlying OSAL file according to `args`.
- Stores a valid newly created stream handle in `*out`.
- The produced stream handle is ready for normal runtime use.
- The produced stream handle must later be destroyed via `stream_destroy()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Returns `STREAM_STATUS_IO_ERROR` if the underlying OSAL file operation fails.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Notes

- The underlying OSAL file is opened through the file operations injected in
  `env`.
- On success, ownership of the newly allocated stream handle is transferred to
  the caller.
- On failure, no stream ownership is transferred to the caller.