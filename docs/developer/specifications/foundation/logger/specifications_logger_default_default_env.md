@page specifications_logger_default_default_env logger_default_default_env() specifications

# Signature

```c
    logger_default_env_t logger_default_default_env(
        stream_t *stream,
        const osal_mem_ops_t *adapter_mem,
        const logger_env_t *port_env);
```

# Purpose

Build a default environment for the `logger_default` adapter.

# Preconditions

- `stream` must point to a valid `stream_t`.
- `adapter_mem` must point to a valid `osal_mem_ops_t`.
- `port_env` must point to a valid `logger_env_t`.

# Invalid arguments

- None.

# Success

- Returns a well-formed `logger_default_env_t`.
- `ret.stream == stream`.
- `ret.adapter_mem == adapter_mem`.
- `ret.port_env == *port_env`.

# Failure

- None.

# Ownership

- The returned `logger_default_env_t` does not take ownership of the provided
  pointers.
- Ownership of all provided dependencies remains with the caller.

# Notes

- This helper packages the provided dependencies into a
  `logger_default_env_t`.
- This helper does not allocate.