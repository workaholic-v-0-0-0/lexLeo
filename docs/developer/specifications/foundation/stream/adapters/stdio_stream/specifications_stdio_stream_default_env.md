@page specifications_stdio_stream_default_env stdio_stream_default_env() specifications

# Signature

```c
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *mem,
    const stream_env_t *port_env);
```

# Purpose

Build a default environment for the `stdio_stream` adapter.

# Preconditions

- `stdio_ops` must designate a valid OSAL stdio operations table.
- `mem` must designate a valid OSAL memory operations table.
- `port_env` must designate a valid `stream` port environment object.

# Invalid arguments

- None.

# Success

- Returns a well-formed `stdio_stream_env_t`.
- Stores `stdio_ops` into `ret.stdio_ops`.
- Stores `mem` into `ret.mem`.
- Copies `*port_env` into `ret.port_env`.

# Failure

- None.

# Notes

- This helper packages borrowed dependencies into a value-initialized
  `stdio_stream_env_t`.
- The returned environment does not take ownership of any pointer passed as
  argument.
- `ret.stdio_ops` provides the borrowed OSAL stdio boundary used by the
  adapter backend to access the standard streams and the stdio primitives
  required to implement the `stream` port contract.
- This helper does not allocate.
