@page specifications_dynamic_buffer_stream_default_env dynamic_buffer_stream_default_env() specifications

# Signature

~~~c
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
    const osal_mem_ops_t *mem_ops);
~~~

# Purpose

Build a default `dynamic_buffer_stream_env_t` from the external borrowed
dependencies required by the `dynamic_buffer_stream` adapter.

# Preconditions

- `mem_ops` points to a valid `osal_mem_ops_t`.

# Invalid arguments

- None.

# Success

- Returns a well-formed `dynamic_buffer_stream_env_t`.
- `ret.mem_ops == mem_ops`.

# Failure

- None.

# Ownership

- The returned environment stores a borrowed reference to the supplied external
  dependency.
- Ownership of `mem_ops` is not transferred.

# Notes

- The dependency stored in `dynamic_buffer_stream_env_t` is the external
  borrowed dependency used by `dynamic_buffer_stream_create()`.
- The owned dynamic buffer is not part of the environment and is initialized
  during completion.
