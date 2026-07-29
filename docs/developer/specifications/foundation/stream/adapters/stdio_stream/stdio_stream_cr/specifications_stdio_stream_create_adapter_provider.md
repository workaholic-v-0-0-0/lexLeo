@page specifications_stdio_stream_create_adapter_provider stdio_stream_create_adapter_provider() specifications

# Signature

```c
stdio_stream_status_t stdio_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env);
```

# Purpose

Create a `stream_adapter_provider_t` for the `stdio_stream` adapter, suitable
for registration in a `stream_factory_t` through
`stream_factory_add_adapter()`.

The produced provider supplies everything required by the stream factory to
construct `stdio_stream` backends and operate them through the `stream` port.

# Preconditions

- `out` must not be `NULL`.
- `cfg` must point to a valid `stdio_stream_cfg_t`.
- `env` must point to a valid `stdio_stream_env_t`.

# Invalid arguments

- None.

# Success

- Returns `STDIO_STREAM_STATUS_OK`.
- Stores a newly created `stream_adapter_provider_t` in `*out`.
- The produced provider is suitable for registration in a `stream_factory_t`
  through `stream_factory_add_adapter()`.

# Failure

- Returns `STDIO_STREAM_STATUS_OOM` if a required allocation fails.
- Leaves `*out` unchanged.
- Releases any resources acquired before the failure.
- No ownership is transferred to the caller.

# Ownership

- Ownership of `cfg` and `env` is not transferred.
- On success, ownership of the produced `stream_adapter_provider_t` is
  transferred to the caller and must be destroyed by
  `stream_destroy_adapter_provider()`.

# Notes

- This function provides the `stdio_stream`-specific adapter provider required
  by the generic stream factory.
- `backend_ctor` defines how `stdio_stream` backends are constructed.
- `vtbl` defines how the constructed backends are operated through the
  `stream` port.
