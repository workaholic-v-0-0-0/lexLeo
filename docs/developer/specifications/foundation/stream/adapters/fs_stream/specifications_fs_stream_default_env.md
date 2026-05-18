@page specifications_fs_stream_default_env fs_stream_default_env() specifications

# Signature

```c
fs_stream_env_t fs_stream_default_env(
    const osal_file_ops_t *file_ops,
    const osal_mem_ops_t *adapter_mem,
    const stream_env_t *port_env);
```

# Purpose

Build a default environment for the `fs_stream` adapter.

# Preconditions

- `file_ops` must point to a valid OSAL file operations table.
- `adapter_mem` must point to a valid OSAL memory operations table.
- `port_env` must point to a valid `stream` port environment object.

# Invalid arguments

- None.

# Success

- Returns a well-formed `fs_stream_env_t`.
- Stores `file_ops` into `ret.file_ops`.
- Stores `adapter_mem` into `ret.adapter_mem`.
- Copies `*port_env` into `ret.port_env`.

# Failure

- None.

# Notes

- This helper packages borrowed dependencies into a value-initialized
  `fs_stream_env_t`.
- The returned environment does not take ownership of any pointer passed as
  argument.
- This helper does not allocate.
