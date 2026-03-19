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

- `stream` must designate a borrowed stream handle obtained through the `stream` API.
- `adapter_mem` must designate a valid OSAL memory operations table.
- `port_env` must designate a valid `logger` port environment object.

# Invalid arguments

- None.

# Success

- Returns a well-formed `logger_default_env_t`.
- `ret.stream == stream`.
- `ret.adapter_mem == adapter_mem`.
- `ret.port_env == *port_env`.

# Failure

- None.

# Notes

- This helper packages borrowed dependencies into a `logger_default_env_t`.
- The returned environment does not take ownership of any pointer passed as
  argument.
- This helper does not allocate.
