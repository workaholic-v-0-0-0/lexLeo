@page specifications_logger_default_env logger_default_env() specifications

# Signature

```c
logger_env_t logger_default_env(const osal_mem_ops_t *mem_ops);
```

# Purpose

Build a default `logger_env_t` from injected memory operations.

# Success

- Returns a `logger_env_t` value `ret` such that `ret.mem == mem_ops`.

# Failure

- None.

# Notes

- This helper preserves the allocator dependency provided by the caller.
- This helper does not allocate and only packages the provided dependency into
  a public `logger_env_t`.
