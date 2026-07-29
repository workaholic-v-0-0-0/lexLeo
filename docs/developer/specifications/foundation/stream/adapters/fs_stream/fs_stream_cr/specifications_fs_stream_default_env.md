@page specifications_fs_stream_default_env fs_stream_default_env() specifications

# Signature

```c
fs_stream_env_t fs_stream_default_env(
    const osal_file_ops_t *file_ops,
    const osal_mem_ops_t *mem_ops);
```

# Purpose

Build a default `fs_stream_env_t` from the external borrowed dependencies
required by the `fs_stream` adapter.

# Preconditions

- `file_ops` points to a valid `osal_file_ops_t`.
- `mem_ops` points to a valid `osal_mem_ops_t`.

# Success

- Returns a well-formed `fs_stream_env_t`.
- `ret.file_ops == file_ops`.
- `ret.mem_ops == mem_ops`.

# Failure

- None.

# Ownership

- The returned environment stores borrowed references to the supplied external
  dependencies.
- Ownership of the supplied dependencies is not transferred.