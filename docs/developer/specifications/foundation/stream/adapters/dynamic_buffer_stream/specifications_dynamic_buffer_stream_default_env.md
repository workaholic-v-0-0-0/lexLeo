@page specifications_dynamic_buffer_stream_default_env dynamic_buffer_stream_default_env() specifications

# Signature

```c
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
    const osal_mem_ops_t *mem,
    const stream_env_t *port_env);
```

# Purpose

Build a default environment for the `dynamic_buffer_stream` adapter.

# Preconditions

- `mem` must point to a valid OSAL memory operations table.
- `port_env` must point to a valid `stream` port environment object.

# Invalid arguments

- None.

# Success

- Returns a well-formed `dynamic_buffer_stream_env_t`.
- Stores `mem` into `ret.mem`.
- Copies `*port_env` into `ret.port_env`.

# Failure

- None.

# Notes

- This helper packages borrowed dependencies into a value-initialized
  `dynamic_buffer_stream_env_t`.
- The returned environment does not take ownership of any pointer passed as
  argument.
- This helper does not allocate.
