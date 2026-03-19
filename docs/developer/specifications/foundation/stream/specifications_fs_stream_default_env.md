@page specifications_fs_stream_default_env fs_stream_default_env() specifications

# Signature

```c
    fs_stream_env_t fs_stream_default_env(
        const osal_file_env_t *file_env,
        const osal_file_ops_t *file_ops,
        const stream_env_t *port_env);
```

# Purpose

Build a default environment for the `fs_stream` adapter.

# Preconditions

- `file_env` must point to a valid OSAL file environment object.
- `file_ops` must point to a valid OSAL file operations table.
- `port_env` must point to a valid `stream` port environment object.

# Invalid arguments

- None.

# Success

- Returns a well-formed `fs_stream_env_t`.
- Copies `*file_env` into `ret.file_env`.
- Stores `file_ops` into `ret.file_ops`.
- Copies `*port_env` into `ret.port_env`.

# Failure

- None.

# Notes

- This helper packages borrowed dependencies into a value-initialized
  `fs_stream_env_t`.
- The returned environment does not take ownership of any pointer passed as
  argument.
- This helper does not allocate.
