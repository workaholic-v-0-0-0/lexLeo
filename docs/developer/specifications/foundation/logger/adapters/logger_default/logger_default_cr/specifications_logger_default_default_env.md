@page specifications_logger_default_default_env logger_default_default_env() specifications

# Signature

```c
logger_default_env_t logger_default_default_env(
    stream_t *stream,
    const osal_time_ops_t *time_ops,
    const osal_mem_ops_t *mem_ops);
```

# Purpose

Build a default `logger_default_env_t` from the external borrowed dependencies
required by the `logger_default` adapter.

# Preconditions

- `stream` points to a valid `stream_t`.
- `time_ops` points to a valid `osal_time_ops_t`.
- `mem_ops` points to a valid `osal_mem_ops_t`.

# Success

- Returns a well-formed `logger_default_env_t`.
- `ret.stream == stream`.
- `ret.time_ops == time_ops`.
- `ret.mem_ops == mem_ops`.

# Failure

- None.

# Ownership

- The returned environment stores borrowed references to the supplied external
  dependencies.
- Ownership of the supplied dependencies is not transferred.
