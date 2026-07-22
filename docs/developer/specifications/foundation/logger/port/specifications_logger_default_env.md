@page specifications_logger_default_env logger_default_env() specifications

# Signature

~~~c
logger_env_t logger_default_env(
    const logger_vtbl_t *vtbl,
    const osal_mem_ops_t *mem_ops);
~~~

# Purpose

Build a default `logger_env_t`.

# Preconditions

- `vtbl` must point to a valid `logger` virtual table.
- `mem_ops` must point to a valid OSAL memory operations table.

# Invalid arguments

- None.

# Success

- Returns a well-formed `logger_env_t`.
- Initializes `ret.vtbl` with `vtbl`.
- Initializes `ret.mem` with `mem_ops`.

# Failure

- None.

# Ownership

- Ownership of the provided dependencies is not transferred.
- The returned environment borrows `vtbl`.
- The returned environment borrows `mem_ops`.

# Notes

- This helper does not allocate.
- This helper performs no validation.
