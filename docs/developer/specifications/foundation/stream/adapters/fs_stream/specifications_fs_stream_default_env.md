@page specifications_fs_stream_default_env fs_stream_default_env() specifications

# Signature

```c
fs_stream_env_t fs_stream_default_env(
    const osal_file_ops_t *file_ops,
    const osal_mem_ops_t *adapter_mem_ops,
    const osal_mem_ops_t *port_mem_ops);
```

# Purpose

Build a default environment for the `fs_stream` adapter.

# Preconditions

- `file_ops` must point to a valid OSAL file operations table.
- `adapter_mem_ops` must point to a valid OSAL memory operations table.
- `port_mem_ops` must point to a valid OSAL memory operations table.

# Invalid arguments

- None.

# Success

- Returns a well-formed `fs_stream_env_t`.
- Stores `file_ops` into `ret.file_ops`.
- Stores `adapter_mem_ops` into `ret.adapter_mem_ops`.
- Stores `port_mem_ops` into `ret.port_mem_ops`.

# Failure

- None.

# Ownership

- Ownership of the provided dependencies is not transferred.
- The returned environment borrows `file_ops`.
- The returned environment borrows `adapter_mem_ops`.
- The returned environment borrows `port_mem_ops`.

# Notes

- This helper does not allocate.