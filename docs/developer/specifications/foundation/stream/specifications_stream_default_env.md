@page specifications_stream_default_env stream_default_env() specifications

# Signature

```c
stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops);
```

# Purpose

Build a default `stream_env_t` from injected memory operations.

# Success

- Returns a `stream_env_t` value `ret` such that `ret.mem == mem_ops`.

# Failure

- None.

# Notes

- This helper preserves the allocator dependency provided by the caller.
- This helper does not allocate and only packages the provided dependency into
  a public `stream_env_t`.
