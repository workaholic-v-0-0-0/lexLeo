@page specifications_stdio_stream_complete_default_init stdio_stream_complete_default_init() specifications

# Signature

~~~c
stdio_stream_status_t stdio_stream_complete_default_init(
    stdio_stream_t *stdio_stream,
    const stdio_stream_cfg_t *cfg,
    const stream_standard_stream_creator_args_t *args);
~~~

# Purpose

Complete the initialization of a `stdio_stream_t` backend by resolving and
attaching its internal borrowed standard stream dependency.

# Preconditions

- `stdio_stream` must designate a valid `stdio_stream_t` previously created by
  `stdio_stream_create()`.
- `cfg` must point to a valid `stdio_stream_cfg_t`.
- `args` must point to a valid `stream_standard_stream_creator_args_t`.
- `stdio_stream` must contain valid external borrowed standard I/O operations.
- The standard I/O operation table must provide valid operations for obtaining
  `stdin`, `stdout`, and `stderr`.
- `args->kind` must be one of:
    - `STREAM_STANDARD_STREAM_KIND_STDIN`
    - `STREAM_STANDARD_STREAM_KIND_STDOUT`
    - `STREAM_STANDARD_STREAM_KIND_STDERR`

# Invalid arguments

- None.

# Success

- Returns `STDIO_STREAM_STATUS_OK`.
- If the wrapped standard stream is already non-`NULL`, leaves it unchanged.
- Otherwise, resolves the requested standard stream from `args->kind` through
  the backend's external borrowed standard I/O operations.
- Attaches the resolved standard stream to the backend as an internal borrowed
  dependency.
- Leaves the backend's external borrowed dependencies unchanged.

# Failure

- None.

# Ownership

- `stdio_stream` remains owned by the caller.
- `cfg` and `args` are borrowed for the duration of the call.
- The standard stream attached to the backend is borrowed.
- Ownership of `stdin`, `stdout`, or `stderr` is not transferred to the
  `stdio_stream_t`.
- No owned resource is acquired by this operation.

# Notes

- This operation completes the backend state not initialized from the external
  borrowed dependencies supplied through `stdio_stream_env_t`.
- In the `stdio_stream` adapter, this completion-time state consists of an
  internal borrowed dependency rather than an owned resource.
- If a standard stream has already been injected into the backend, this
  operation preserves it and does not replace it with a default standard
  stream.
- `cfg` currently carries no effective configuration for this initialization
  step.
