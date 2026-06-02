@page specifications_stdio_stream_default_env stdio_stream_default_env() specifications

# Signature

```c
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *adapter_mem_ops,
    const osal_mem_ops_t *port_mem_ops);
```

# Purpose

Build a default environment for the `stdio_stream` adapter.

# Preconditions

- `stdio_ops` must point to a valid OSAL stdio operations table.
- `adapter_mem_ops` must point to a valid OSAL memory operations table.
- `port_mem_ops` must point to a valid OSAL memory operations table.

# Invalid arguments

- None.

# Success

- Returns a well-formed `stdio_stream_env_t`.
- Stores `stdio_ops` into `ret.stdio_ops`.
- Stores `adapter_mem_ops` into `ret.adapter_mem_ops`.
- Stores `port_mem_ops` into `ret.port_mem_ops`.

# Failure

- None.

# Ownership

- Ownership of the provided dependencies is not transferred.
- The returned environment borrows `stdio_ops`.
- The returned environment borrows `adapter_mem_ops`.
- The returned environment borrows `port_mem_ops`.

# Notes

- This helper does not allocate.
