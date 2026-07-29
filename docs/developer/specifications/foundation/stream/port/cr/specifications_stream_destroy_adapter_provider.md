@page specifications_stream_destroy_adapter_provider stream_destroy_adapter_provider() specifications

# Signature

```c
void stream_destroy_adapter_provider(
    stream_adapter_provider_t *adapter_provider);
```

# Purpose

Destroy a `stream_adapter_provider_t`.

# Preconditions

- `adapter_provider` must designate a valid `stream_adapter_provider_t`.

# Success

- Releases the adapter provider and all resources it owns.

# Failure

- None.

# Ownership

- Ownership of `adapter_provider` is consumed.
