@page specifications_dynamic_buffer_stream_create_adapter_provider dynamic_buffer_stream_create_adapter_provider() specifications

# Signature

```c
dynamic_buffer_stream_status_t dynamic_buffer_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env);
```

# Purpose

Create a `stream_adapter_provider_t` able to construct fully initialized
`dynamic_buffer_stream_t` backends from adapter configuration and external borrowed
dependencies.

# Preconditions

- `out` is not `NULL`.
- `cfg` points to a valid `dynamic_buffer_stream_cfg_t`.
- `env` points to a valid `dynamic_buffer_stream_env_t`.

# Invalid arguments

- None.

# Success

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`.
- Stores a newly created `stream_adapter_provider_t` in `*out`.
- Copies the adapter configuration required to construct
  `dynamic_buffer_stream_t` backends.
- Copies the external borrowed dependencies from `env` into provider-owned
  constructor user data.
- Configures the provider to:
    - create partially initialized `dynamic_buffer_stream_t` backends using
      `dynamic_buffer_stream_create()`;
    - complete their initialization using
      `dynamic_buffer_stream_complete_default_init()`;
    - bind them to the `stream` port using the dispatch table returned by
      `dynamic_buffer_stream_vtbl()`.
- The resulting provider can be registered with a `stream` factory to construct
  dynamic-buffer-backed streams.

# Failure

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM` if allocation of the provider or
  its provider-owned data fails.
- Leaves `*out` unchanged.
- Releases any intermediate owned resources allocated by this operation before
  returning.

# Ownership

- Ownership of `cfg` and `env` is not transferred.
- Ownership of the external borrowed dependencies referenced by `env` is not
  transferred.
- The provider owns any configuration and constructor data allocated or copied
  for its operation.
- On success, ownership of the produced `stream_adapter_provider_t` is
  transferred to the caller.
- Resources owned by the provider are released when the provider is destroyed.

# Notes

- The provider captures the information required to construct
  `dynamic_buffer_stream_t` backends without requiring the original `cfg` or
  `env` objects to remain alive.
- External dependencies referenced through the copied environment remain
  borrowed and must remain valid for as long as the provider may use them.
- Backend creation performed through the provider follows the same two-phase
  initialization model as direct construction:
    - `dynamic_buffer_stream_create()` initializes external borrowed
      dependencies.
    - `dynamic_buffer_stream_complete_default_init()` initializes the remaining
      backend state, including the owned dynamic buffer resource.
