@page specifications_stdio_stream_default_env stdio_stream_default_env() specifications

# Signature

~~~c
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *mem_ops);
~~~

# Purpose

Build a default `stdio_stream_env_t` from the external borrowed dependencies
required by the `stdio_stream` adapter.

# Preconditions

- `stdio_ops` points to a valid `osal_stdio_ops_t`.
- `mem_ops` points to a valid `osal_mem_ops_t`.

# Invalid arguments

- None.

# Success

- Returns a well-formed `stdio_stream_env_t`.
- `ret.stdio_ops == stdio_ops`.
- `ret.mem_ops == mem_ops`.

# Failure

- None.

# Ownership

- The returned environment stores borrowed references to the supplied external
  dependencies.
- Ownership of `stdio_ops` is not transferred.
- Ownership of `mem_ops` is not transferred.

# Notes

- The dependencies stored in `stdio_stream_env_t` are the external borrowed
  dependencies used by `stdio_stream_create()`.
- Internal borrowed dependencies, such as the wrapped standard stream, are not
  part of the environment and are initialized during completion.
