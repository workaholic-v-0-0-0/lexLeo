@page specifications_stdio_stream_create_stream stdio_stream_create_stream() specifications

# Signature

~~~c
stream_status_t stdio_stream_create_stream(
    stream_t **out,
    const stdio_stream_args_t *args,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env);
~~~

# Purpose

Create a `stdio_stream` instance directly.

# Preconditions

- If `args != NULL`, `args` must point to a valid
  `stdio_stream_args_t`.
- If `cfg != NULL`, `cfg` must point to a valid
  `stdio_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid
  `stdio_stream_env_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `args` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- `args->kind` must be one of:
    - `STDIO_STREAM_KIND_STDIN`
    - `STDIO_STREAM_KIND_STDOUT`
    - `STDIO_STREAM_KIND_STDERR`

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created stream handle in `*out`.
- The created stream wraps one of:
    - `stdin` when `args->kind == STDIO_STREAM_KIND_STDIN`
    - `stdout` when `args->kind == STDIO_STREAM_KIND_STDOUT`
    - `stderr` when `args->kind == STDIO_STREAM_KIND_STDERR`

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

- On success, ownership of the newly created stream handle is transferred to the
  caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.

# Notes

- The adapter wraps existing standard C streams (`stdin`, `stdout`, `stderr`)
  and does not take ownership of them.
- The `cfg` parameter is currently reserved for future extensions.
