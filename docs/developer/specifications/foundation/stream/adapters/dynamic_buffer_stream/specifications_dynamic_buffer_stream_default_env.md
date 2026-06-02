@page specifications_dynamic_buffer_stream_default_env dynamic_buffer_stream_default_env() specifications

# Signature

```c
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
    const osal_mem_ops_t *adapter_mem_ops,
    const osal_mem_ops_t *port_mem_ops);
```

# Purpose

Build a default environment for the `dynamic_buffer_stream` adapter.

# Preconditions

- `adapter_mem_ops` must point to a valid OSAL memory operations table.
- `port_mem_ops` must point to a valid OSAL memory operations table.

# Invalid arguments

- None.

# Success

- Returns a well-formed `dynamic_buffer_stream_env_t`.
- Stores `adapter_mem_ops` into `ret.adapter_mem_ops`.
- Stores `port_mem_ops` into `ret.port_mem_ops`.

# Failure

- None.

# Ownership

- Ownership of the provided dependencies is not transferred.
- The returned environment borrows `adapter_mem_ops`.
- The returned environment borrows `port_mem_ops`.

# Notes

- This helper does not allocate.
