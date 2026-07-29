@page specifications_lexleo_app_default_env lexleo_app_default_env() specifications

# Signature

```c
lexleo_app_env_t lexleo_app_default_env(
    const osal_mem_ops_t *mem_ops,
    const osal_stdio_ops_t *stdio_ops,
    const osal_file_ops_t *file_ops,
    const osal_str_ops_t *str_ops,
    const osal_time_ops_t *time_ops
);
```

# Purpose

Build a LexLeo application environment from borrowed runtime dependencies.

# Preconditions

- The supplied dependencies must satisfy the requirements of the
  application services that will use the returned environment.

# Success

- Returns a `lexleo_app_env_t` value such that:
    - `env.mem_ops == mem_ops`.
    - `env.stdio_ops == stdio_ops`.
    - `env.file_ops == file_ops`.
    - `env.str_ops == str_ops`.
    - `env.time_ops == time_ops`.

# Failure

- This function cannot fail.

# Ownership

- Ownership of the supplied dependencies is not transferred.
- The returned environment stores borrowed pointers to those dependencies.

# Notes

- This function does not allocate memory or initialize runtime resources.
- This function does not validate the supplied dependencies.
