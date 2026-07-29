@page specifications_dynamic_buffer_stream_create dynamic_buffer_stream_create() specifications

# Signature

```c
dynamic_buffer_stream_status_t dynamic_buffer_stream_create(
    dynamic_buffer_stream_t **out,
    const dynamic_buffer_stream_env_t *env);
```

# Purpose

Create a partially initialized `dynamic_buffer_stream_t` backend from its
external borrowed dependencies.

# Preconditions

- `out` is not `NULL`.
- `env` points to a valid `dynamic_buffer_stream_env_t`.

# Success

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`.
- Stores a newly created `dynamic_buffer_stream_t` in `*out`.
- Initializes the backend's external borrowed dependencies from `env`,
  including its memory operations.
- Leaves the backend's owned dynamic buffer resource uninitialized.

# Failure

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `env` and the external borrowed dependencies supplied through
  it is not transferred.
- On success, ownership of the produced `dynamic_buffer_stream_t` is
  transferred to the caller.
- The produced `dynamic_buffer_stream_t` must later be released via
  `dynamic_buffer_stream_t::close()`.

# Notes

- The initialization of the produced `dynamic_buffer_stream_t` must be
  completed by `dynamic_buffer_stream_complete_default_init()` before it can
  be used.
- `dynamic_buffer_stream_create()` initializes the backend's external borrowed
  dependencies.
- Internal borrowed dependencies and owned resources, when applicable, are
  initialized by `dynamic_buffer_stream_complete_default_init()`.
- In the `dynamic_buffer_stream` adapter, the dynamic buffer is an owned
  resource and is therefore not created by this operation.
