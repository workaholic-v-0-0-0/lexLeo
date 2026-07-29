@page specifications_stdio_stream_create stdio_stream_create() specifications

# Signature

~~~c
stdio_stream_status_t stdio_stream_create(
    stdio_stream_t **out,
    const stdio_stream_env_t *env);
~~~

# Purpose

Create a partially initialized `stdio_stream_t` backend from its external
borrowed dependencies.

# Preconditions

- `out` must not be `NULL`.
- `env` must point to a valid `stdio_stream_env_t`.

# Invalid arguments

- None.

# Success

- Returns `STDIO_STREAM_STATUS_OK`.
- Stores a newly created `stdio_stream_t` in `*out`.
- Initializes the backend's external borrowed dependencies from `env`,
  including its standard I/O operations and memory operations.
- Leaves the backend's internal borrowed standard stream uninitialized.

# Failure

- Returns `STDIO_STREAM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `env` and the external borrowed dependencies supplied through
  it is not transferred.
- On success, ownership of the produced `stdio_stream_t` is transferred to
  the caller.
- The produced `stdio_stream_t` must later be released through the close
  operation exposed by `stdio_stream_vtbl()`.

# Notes

- The initialization of the produced `stdio_stream_t` must be completed by
  `stdio_stream_complete_default_init()` before it can be used.
- `stdio_stream_create()` initializes the backend's external borrowed
  dependencies.
- Internal borrowed dependencies and owned resources, when applicable, are
  initialized by `stdio_stream_complete_default_init()`.
- In the `stdio_stream` adapter, the wrapped standard stream is an internal
  borrowed dependency and is therefore not attached by this operation.
